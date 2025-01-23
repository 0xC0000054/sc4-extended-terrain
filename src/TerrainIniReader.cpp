////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-extended-terrain, a DLL Plugin for SimCity 4
// that adds support for more terrain types.
//
// Copyright (c) 2024, 2025 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
////////////////////////////////////////////////////////////////////////////

#include "TerrainIniReader.h"
#include "DebugUtil.h"
#include "Logger.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/ini_parser.hpp"
#include "cGZPersistResourceKey.h"
#include "cIGZCOM.h"
#include "cIGZPersistDBRecord.h"
#include "cIGZPersistDBSerialRecord.h"
#include "cIGZDBSerialRecordAsIGZFile.h"
#include "cIGZPersistResourceManager.h"
#include "GZFileStreamBufAdapter.h"
#include "GZServPtrs.h"
#include "cRZAutoRefCount.h"
#include "cRZCOMDllDirector.h"
#include "frozen/unordered_set.h"
#include "StringViewUtil.h"

#include <array>
#include <memory>
#include <utility>

namespace
{
	static constexpr std::string_view kMiscTextures = "MiscTextures";
	static const std::array<std::pair<std::string, TerrainTexture>, 6> AdditionalMiscTextures =
	{
		std::make_pair("CutawayTop", TerrainTexture::CutawayTop),
		std::make_pair("CutawayMiddle", TerrainTexture::CutawayMiddle),
		std::make_pair("CutawayBottom", TerrainTexture::CutawayBottom),
		std::make_pair("InshoreWater", TerrainTexture::InshoreWater),
		std::make_pair("MidshoreWater", TerrainTexture::MidshoreWater),
		std::make_pair("OutshoreWater", TerrainTexture::OutshoreWater),
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

	std::unique_ptr<GZFileStreamBufAdapter> OpenDBPFRecordAsStreamBuf(const cGZPersistResourceKey& key)
	{
		std::unique_ptr<GZFileStreamBufAdapter> adapter;

		cIGZCOM* const pCOM = RZGetFramework()->GetCOMObject();
		cIGZPersistResourceManagerPtr pResourceManager;

		if (pCOM && pResourceManager)
		{
			cRZAutoRefCount<cIGZPersistDBRecord> pRecord;

			if (pResourceManager->OpenDBRecord(key, pRecord.AsPPObj(), false))
			{
				cRZAutoRefCount<cIGZPersistDBSerialRecord> pSerialRecord;

				if (pRecord->QueryInterface(GZIID_cIGZPersistDBSerialRecord, pSerialRecord.AsPPVoid()))
				{
					cRZAutoRefCount<cIGZDBSerialRecordAsIGZFile> pAsIGZFile;

					if (pCOM->GetClassObject(
						GZCLSID_cIGZDBSerialRecordAsIGZFile,
						GZIID_cIGZDBSerialRecordAsIGZFile,
						pAsIGZFile.AsPPVoid()))
					{
						if (pAsIGZFile->SetDBSerialRecord(pSerialRecord))
						{
							cRZAutoRefCount<cIGZFile> pFile;

							if (pAsIGZFile->QueryInterface(GZIID_cIGZFile, pFile.AsPPVoid()))
							{
								adapter = std::make_unique<GZFileStreamBufAdapter>(std::move(pFile));
							}
						}
					}
				}
			}
		}

		return adapter;
	}

	boost::property_tree::ptree LoadTerrainIni()
	{
		boost::property_tree::ptree tree;

		// The TGI of the Terrain.ini entry.
		const cGZPersistResourceKey key(0x00000000, 0x8A5971C5, 0xAA597172);

		std::unique_ptr<GZFileStreamBufAdapter> adapter = OpenDBPFRecordAsStreamBuf(key);

		if (adapter)
		{
			std::istream file(adapter.get());

			boost::property_tree::ini_parser::read_ini(file, tree);
		}

		return tree;
	}

	void InitializeTerrainNames(
		const boost::property_tree::ptree& terrainIni,
		const std::vector<std::string>& terrainPrefixes,
		TerrainNameCollection& terrainNames)
	{
		terrainNames.clear();

		auto optionalDisplayNames = terrainIni.get_child_optional("DisplayNames");

		if (optionalDisplayNames.has_value())
		{
			auto& displayNames = optionalDisplayNames.get();

			if (!displayNames.empty())
			{
				for (auto& prefix : terrainPrefixes)
				{
					auto optionalDisplayName = displayNames.get_optional<std::string>(prefix);

					if (optionalDisplayName)
					{
						terrainNames.emplace_back(prefix, optionalDisplayName.get());
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
		const boost::property_tree::ptree& terrainIni,
		const std::vector<std::string>& terrainPrefixes,
		ITerrainTextureRedirectManager& redirectManager)
	{
		Logger& logger = Logger::GetInstance();

		auto optionalTextureOffsets = terrainIni.get_child_optional("TextureOffsets");

		if (optionalTextureOffsets.has_value())
		{
			auto& textureOffsetsSection = optionalTextureOffsets.get();

			if (!textureOffsetsSection.empty())
			{
				for (auto& prefix : terrainPrefixes)
				{
					auto optionalTextureOffset = textureOffsetsSection.get_optional<std::string>(prefix);

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
		const boost::property_tree::ptree& terrainIni,
		const std::vector<std::string>& terrainPrefixes,
		ITerrainTextureRedirectManager& redirectManager)
	{
		Logger& logger = Logger::GetInstance();

		for (auto& prefix : terrainPrefixes)
		{
			const std::string sectionName = std::string(prefix).append(kMiscTextures);

			const auto& miscTextureSection = terrainIni.get_child(sectionName);

			// The default MiscTexture format only has 3 fields.
			if (miscTextureSection.size() > 3)
			{
				for (const auto& additionalTextureItem : AdditionalMiscTextures)
				{
					auto sectionField = miscTextureSection.find(additionalTextureItem.first);

					if (sectionField != miscTextureSection.not_found())
					{
						uint32_t textureID = 0;

						if (StringViewUtil::TryParse(sectionField->second.data(), textureID))
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

	void InitializeTerrainTextureRedirects(
		const boost::property_tree::ptree& terrainIni,
		const std::vector<std::string>& terrainPrefixes,
		ITerrainTextureRedirectManager& redirectManager)
	{
		ReadTerrainTextureOffsets(terrainIni, terrainPrefixes, redirectManager);
		ReadAdditionalMiscTerrainTextures(terrainIni, terrainPrefixes, redirectManager);
	}
}

void TerrainIniReader::Parse(
	TerrainNameCollection& terrainNames,
	ITerrainTextureRedirectManager& textureRedirectManager)
{
	Logger& logger = Logger::GetInstance();

	terrainNames.clear();

	try
	{
		boost::property_tree::ptree tree = LoadTerrainIni();

		if (!tree.empty())
		{
			std::vector<std::string> terrainPrefixes;

			for (const auto& section : tree)
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

					const auto textureMapTable = tree.get_child_optional(textureMapTableName);

					// Skip any MiscTexture sections that don't have a TextureMapTable.
					if (textureMapTable.has_value())
					{
						terrainPrefixes.emplace_back(prefix);
					}
				}
			}

			InitializeTerrainNames(tree, terrainPrefixes, terrainNames);
			InitializeTerrainTextureRedirects(tree, terrainPrefixes, textureRedirectManager);
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
