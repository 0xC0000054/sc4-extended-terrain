/*
 * This file is part of sc4-extended-terrain, a DLL Plugin for SimCity 4
 * that adds support for more terrain types.
 *
 * Copyright (C) 2024, 2025 Nicholas Hayes
 *
 * sc4-extended-terrain is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * sc4-extended-terrain is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with sc4-extended-terrain.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "TerrainIniReader.h"
#include "DebugUtil.h"
#include "Logger.h"
#include "cGZPersistResourceKey.h"
#include "frozen/unordered_set.h"
#include "IniReader.h"
#include "StringViewUtil.h"

#include <array>
#include <memory>
#include <utility>

namespace
{
	static constexpr std::string_view kMiscTextures = "MiscTextures";
	static const std::array<std::pair<std::string, TerrainTexture>, 9> AdditionalMiscTextures =
	{
		std::make_pair("CutawayTop", TerrainTexture::CutawayTop),
		std::make_pair("CutawayMiddle", TerrainTexture::CutawayMiddle),
		std::make_pair("CutawayBottom", TerrainTexture::CutawayBottom),
		std::make_pair("InshoreWater", TerrainTexture::InshoreWater),
		std::make_pair("MidshoreWater", TerrainTexture::MidshoreWater),
		std::make_pair("OutshoreWater", TerrainTexture::OutshoreWater),
		std::make_pair("WaterBoxGradient", TerrainTexture::WaterBoxGradient),
		std::make_pair("WaterColorDepthGradient", TerrainTexture::WaterColorDepthGradient),
		std::make_pair("WaterGlareMask", TerrainTexture::WaterGlareMask),
	};

	// These values are taken from TGI values with a type of 0x7AB50E44 and a group of 0x891B0E1A.
	// Each item is the upper 20 bits of the instance id, when that value is non-zero.
	// To eliminate the chance of conflicts, we prevent texture offsets that match these values
	// from being used.
	static constexpr frozen::unordered_set<uint32_t, 42> SC4ReservedTextureOffsets =
	{
		0x08F65000,
		0x09187000,
		0x095A8000,
		0x09909000,
		0x0999D000,
		0x0A051000,
		0x0A0B4000,
		0x28EB8000,
		0x28F7A000,
		0x292C4000,
		0x2A05B000,
		0x4A052000,
		0x695C0000,
		0x699CA000,
		0x69EFB000,
		0x69F00000,
		0x69F10000,
		0x69F40000,
		0x69F50000,
		0x69F80000,
		0x69F90000,
		0x69FA0000,
		0x69FB0000,
		0x6A05B000,
		0x6A6C9000,
		0x6A849000,
		0x88F7A000,
		0x8A050000,
		0xA9860000,
		0xAA050000,
		0xC9909000,
		0xC9EFB000,
		0xCA050000,
		0xCA131000,
		0xCA5CC000,
		0xE8EA9000,
		0xE8F79000,
		0xE92C4000,
		0xE95A8000,
		0xE9860000,
		0xE9A44000,
		0xEA05D000,
	};

	void InitializeTerrainNames(
		const IniReader& terrainIni,
		const std::vector<std::string>& terrainPrefixes,
		TerrainNameCollection& terrainNames)
	{
		terrainNames.clear();

		auto optionalDisplayNames = terrainIni.get_section_optional("DisplayNames");

		if (optionalDisplayNames.has_value())
		{
			auto& displayNames = optionalDisplayNames.value();

			if (!displayNames.empty())
			{
				for (auto& prefix : terrainPrefixes)
				{
					auto optionalDisplayName = displayNames.get_value_optional(prefix);

					if (optionalDisplayName)
					{
						terrainNames.emplace_back(prefix, optionalDisplayName.value());
					}
					else
					{
						terrainNames.emplace_back(prefix);
					}
				}
			}
		}

		if (terrainNames.empty())
		{
			for (auto& prefix : terrainPrefixes)
			{
				terrainNames.emplace_back(prefix);
			}
		}
	}

	void ReadTerrainTextureOffsets(
		const IniReader& terrainIni,
		const std::vector<std::string>& terrainPrefixes,
		ITerrainTextureRedirectManager& redirectManager)
	{
		Logger& logger = Logger::GetInstance();

		auto optionalTextureOffsets = terrainIni.get_section_optional("TextureOffsets");

		if (optionalTextureOffsets.has_value())
		{
			auto& textureOffsetsSection = optionalTextureOffsets.value();

			if (!textureOffsetsSection.empty())
			{
				for (auto& prefix : terrainPrefixes)
				{
					auto optionalTextureOffset = textureOffsetsSection.get_value_optional(prefix);

					if (optionalTextureOffset.has_value())
					{
						uint32_t offset = 0;

						if (StringViewUtil::TryParse(optionalTextureOffset.value(), offset))
						{
							// A texture offset of 0 is treated as a no-op because SC4 terrain textures
							// use it by default.
							if (offset != 0)
							{
								// Maxis terrain textures use the following format:
								//
								// 0x00000ZTT
								//
								// Z  - the zoom level (0-4).
								// TT - the texture id.
								//
								// Because Maxis set the upper 20 bits of their terrain texture ids to 0, we use those
								// 20 bits as an offset to separate terrain textures that use the same ZTT values.
								// The Texture Index id is used for the upper 16 bits of the 20 bit offset code.
								//
								// 0xABCDEZTT
								//
								// ABCD - the user's Texture Index id.
								// E    - reserved.
								// Z    - the zoom level (0-4).
								// TT   - the texture id.
								if ((offset & 0x00000fff) == 0)
								{
									if (SC4ReservedTextureOffsets.contains(offset & 0xfffff000))
									{
										logger.WriteLineFormatted(
											LogLevel::Error,
											"The %s texture offset value 0x%08x is used by other SimCity 4 textures.",
											prefix.c_str(),
											offset & 0xffff0000);
									}
									else
									{
										redirectManager.AddTextureIDOffset(prefix, offset);
									}
								}
								else
								{
									logger.WriteLineFormatted(
										LogLevel::Error,
										"Invalid %s texture offset (0x%08x). It must use the form 0xABCDE000.",
										prefix.c_str(),
										offset);
								}
							}
						}
					}
				}
			}
		}
	}

	void ReadAdditionalMiscTerrainTextures(
		const IniReader& terrainIni,
		const std::vector<std::string>& terrainPrefixes,
		ITerrainTextureRedirectManager& redirectManager)
	{
		Logger& logger = Logger::GetInstance();

		for (auto& prefix : terrainPrefixes)
		{
			const std::string sectionName = std::string(prefix).append(kMiscTextures);

			const auto& optionalMiscTextures = terrainIni.get_section_optional(sectionName);

			// The default MiscTexture format only has 3 fields.
			if (optionalMiscTextures.has_value())
			{
				const auto& miscTextureSection = optionalMiscTextures.value();

				for (const auto& additionalTextureItem : AdditionalMiscTextures)
				{
					auto sectionField = miscTextureSection.find(additionalTextureItem.first);

					if (sectionField != miscTextureSection.not_found())
					{
						uint32_t textureID = 0;

						if (StringViewUtil::TryParse(sectionField->second, textureID))
						{
							// The upper 20 bits of the texture id must be 0 for the texture offset system to work.
							// See ReadTerrainTextureOffsets for a description of the format.
							// We also check that bits 24-28 are 0 because SC4 uses it for the 0-4 zoom level.
							if (textureID != 0 && (textureID & 0xffffff00) == 0)
							{
								redirectManager.AddTextureRedirect(prefix, additionalTextureItem.second, textureID);
							}
							else
							{
								logger.WriteLineFormatted(
									LogLevel::Error,
									"Invalid %s %s texture id (0x%08x). It must use the form 0x000000TT.",
									prefix.c_str(),
									sectionField->first.c_str(),
									textureID);
							}
						}
					}
				}
			}
		}

	}

	void ReadFloraBiomeFamilies(
		const IniReader& terrainIni,
		const std::vector<std::string>& terrainPrefixes,
		FloraBiomeManager& floraBiomeManager)
	{
		Logger& logger = Logger::GetInstance();

		auto optionalSection = terrainIni.get_section_optional("FloraBiomeFamilies");

		if (optionalSection.has_value())
		{
			auto& section = optionalSection.value();

			for (auto& prefix : terrainPrefixes)
			{
				auto optionalValue = section.get_value_optional(prefix);

				if (optionalValue.has_value())
				{
					uint32_t familyID = 0;

					if (StringViewUtil::TryParse(optionalValue.value(), familyID) && familyID != 0)
					{
						floraBiomeManager.AddBiomeFamily(prefix, familyID);
					}
					else
					{
						logger.WriteLineFormatted(
							LogLevel::Error,
							"Invalid %s flora biome family ID: %s.",
							prefix.c_str(),
							optionalValue.value().c_str());
					}
				}
			}
		}
	}

	void ReadTuningExemplarInstances(
		const IniReader& terrainIni,
		const std::vector<std::string>& terrainPrefixes,
		const char* sectionName,
		TuningExemplarManager::ExemplarKind kind,
		TuningExemplarManager& tuningExemplarManager)
	{
		Logger& logger = Logger::GetInstance();

		auto optionalSection = terrainIni.get_section_optional(sectionName);

		if (optionalSection.has_value())
		{
			auto& section = optionalSection.value();

			for (auto& prefix : terrainPrefixes)
			{
				auto optionalValue = section.get_value_optional(prefix);

				if (optionalValue.has_value())
				{
					uint32_t instanceID = 0;

					if (StringViewUtil::TryParse(optionalValue.value(), instanceID))
					{
						tuningExemplarManager.AddExemplarInstance(kind, prefix, instanceID);
					}
					else
					{
						logger.WriteLineFormatted(
							LogLevel::Error,
							"Invalid %s instance ID in %s: %s.",
							prefix.c_str(),
							sectionName,
							optionalValue.value().c_str());
					}
				}
			}
		}
	}

	void InitializeTerrainTextureRedirects(
		const IniReader& terrainIni,
		const std::vector<std::string>& terrainPrefixes,
		ITerrainTextureRedirectManager& redirectManager)
	{
		ReadTerrainTextureOffsets(terrainIni, terrainPrefixes, redirectManager);
		ReadAdditionalMiscTerrainTextures(terrainIni, terrainPrefixes, redirectManager);
	}
}

void TerrainIniReader::Parse(
	TerrainNameCollection& terrainNames,
	ITerrainTextureRedirectManager& textureRedirectManager,
	FloraBiomeManager& floraBiomeManager,
	TuningExemplarManager& tuningExemplarManager)
{
	Logger& logger = Logger::GetInstance();

	terrainNames.clear();

	try
	{
		// The TGI of the Terrain.ini entry.

		const cGZPersistResourceKey key(0x00000000, 0x8A5971C5, 0xAA597172);

		IniReader iniReader(key);

		if (!iniReader.empty())
		{
			std::vector<std::string> terrainPrefixes;

			for (const auto& section : iniReader)
			{
				const std::string& sectionName = section.first;

				// The TerrainMiscTextures section is excluded because it is shared between
				// the other MiscTexture sections.
				if (sectionName.ends_with(kMiscTextures)
					&& sectionName.size() > kMiscTextures.size()
					&& sectionName != "TerrainMiscTextures")
				{
					const std::string prefix = sectionName.substr(0, sectionName.size() - kMiscTextures.size());

					const std::string textureMapTableName = prefix + "TextureMapTable";

					const auto textureMapTable = iniReader.get_section_optional(textureMapTableName);

					// Skip any MiscTexture sections that don't have a TextureMapTable.
					if (textureMapTable.has_value())
					{
						terrainPrefixes.emplace_back(prefix);
					}
				}
			}

			InitializeTerrainNames(iniReader, terrainPrefixes, terrainNames);
			InitializeTerrainTextureRedirects(iniReader, terrainPrefixes, textureRedirectManager);
			ReadFloraBiomeFamilies(iniReader, terrainPrefixes, floraBiomeManager);
			ReadTuningExemplarInstances(
				iniReader,
				terrainPrefixes,
				"TerrainPropertiesExemplars",
				TuningExemplarManager::ExemplarKind::TerrainProperties,
				tuningExemplarManager);
			ReadTuningExemplarInstances(
				iniReader,
				terrainPrefixes,
				"FloraTuningExemplars",
				TuningExemplarManager::ExemplarKind::FloraTuning,
				tuningExemplarManager);
		}
		else
		{
			logger.WriteLine(
				LogLevel::Error,
				"Failed to open the Terrain INI file.");
		}
	}
	catch (const std::exception& e)
	{
		logger.WriteLineFormatted(
			LogLevel::Error,
			"Error reading the Terrain INI file: %s",
			e.what());
	}
}
