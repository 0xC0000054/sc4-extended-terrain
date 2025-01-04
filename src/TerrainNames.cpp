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

#include "TerrainNames.h"
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

#include <memory>

namespace
{
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
}

void TerrainNames::Load(std::vector<TerrainNameEntry>& entries)
{
	Logger& logger = Logger::GetInstance();

	entries.clear();

	try
	{
		boost::property_tree::ptree tree = LoadTerrainIni();

		if (!tree.empty())
		{
			const std::string_view kMiscTextures = "MiscTextures";

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
						entries.emplace_back(prefix);
					}
				}
			}
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
