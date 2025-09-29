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

#include "RegionTerrainData.h"
#include "boost/endian.hpp"
#include "cISC4App.h"
#include "cISC4Region.h"
#include "GZStringConvert.h"
#include "SC4String.h"

#include <array>
#include <fstream>

using namespace std::string_view_literals;

namespace
{
	cRZBaseString GetRegionRootFolderCore(cISC4App& sc4App)
	{
		cRZBaseString folder;

		sc4App.GetRegionsDirectory(folder);

		return folder;
	}

	std::filesystem::path GetRegionRootFolder(cISC4App& sc4App)
	{
		static std::filesystem::path path(GZStringConvert::ToFileSystemPath(GetRegionRootFolderCore(sc4App)));

		return path;
	}

	std::filesystem::path GetTerrainDataFilePath(cISC4App& sc4App)
	{
		std::filesystem::path path = GetRegionRootFolder(sc4App);

		path /= GZStringConvert::ToFileSystemPath(*sc4App.GetRegion()->GetDirectoryName()->AsIGZString());
		path /= L"TerrainType.bin"sv;

		return path;
	}

	uint32_t ReadUint32LittleEndian(std::ifstream& stream)
	{
		std::array<unsigned char, 4> buffer{};

		stream.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

		return boost::endian::endian_load<uint32_t, 4, boost::endian::order::little>(buffer.data());
	}

	void WriteUint32LittleEndian(std::ofstream& stream, uint32_t value)
	{
		std::array<unsigned char, 4> buffer{};

		boost::endian::endian_store<uint32_t, 4, boost::endian::order::little>(buffer.data(), value);

		stream.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
	}

	constexpr std::array<char, 4> kHeaderSignature = { 'T', 'T', 'Y', 'P' }; // Terrain Type
	constexpr size_t kHeaderSignatureLength = kHeaderSignature.size();
	constexpr uint32_t kVersion = 1;

	struct Header
	{
		char signature[kHeaderSignatureLength];
		boost::endian::little_uint32_t version;

		Header()
			: signature{},
			  version(0)
		{
		}

		Header(std::array<char, 4> sig, uint32_t version)
			: signature{},
			  version(version)
		{
			memcpy(signature, sig.data(), sig.size());
		}
	};
}

bool RegionTerrainData::Load(cISC4App& sc4App, cRZBaseString& sectionPrefix)
{
	bool result = false;

	const std::filesystem::path path = GetTerrainDataFilePath(sc4App);

	std::ifstream stream(path, std::ofstream::binary);

	if (stream)
	{
		// Read the header

		Header header;
		stream.read(reinterpret_cast<char*>(&header), sizeof(header));

		if (memcmp(header.signature, kHeaderSignature.data(), kHeaderSignature.size()) == 0
			&& header.version == 1)
		{
			// Read the data

			const uint32_t stringLengthInBytes = ReadUint32LittleEndian(stream);

			sectionPrefix.Resize(stringLengthInBytes);
			stream.read(sectionPrefix.Data(), stringLengthInBytes);
			result = true;
		}
	}

	return result;
}

bool RegionTerrainData::Save(cISC4App& sc4App, const cRZBaseString& sectionPrefix)
{
	bool result = false;

	const std::filesystem::path path = GetTerrainDataFilePath(sc4App);

	std::ofstream stream(path, std::ofstream::trunc | std::ofstream::binary);

	if (stream)
	{
		// Write the header

		Header header(kHeaderSignature, kVersion);
		stream.write(reinterpret_cast<const char*>(&header), sizeof(header));

		// Write the data
		WriteUint32LittleEndian(stream, sectionPrefix.Strlen());
		stream.write(sectionPrefix.Data(), sectionPrefix.Strlen());

		result = true;
	}

	return result;
}
