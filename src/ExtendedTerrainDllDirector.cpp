////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-extended-terrain, a DLL Plugin for SimCity 4
// that adds support for more terrain types.
//
// Copyright (c) 2024 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
////////////////////////////////////////////////////////////////////////////

#include "cIGZCOM.h"
#include "cIGZCheatCodeManager.h"
#include "cIGZFrameWork.h"
#include "cISC4App.h"
#include "cISC4City.h"
#include "cIGZMessageServer2.h"
#include "cIGZMessageTarget.h"
#include "cIGZMessageTarget2.h"
#include "cIGZString.h"
#include "cIGZWinMgr.h"
#include "cRZMessage2COMDirector.h"
#include "cRZMessage2Standard.h"
#include "cRZBaseString.h"
#include "GZServPtrs.h"

#include "ExtendedTerrainWinManager.h"
#include "FileSystem.h"
#include "Logger.h"
#include "SC4VersionDetection.h"
#include "version.h"

static constexpr uint32_t kExtendedTerrainDirectorID = 0x7DBE96D9;

class ExtendedTerrainDllDirector : public cRZCOMDllDirector
{
public:
	ExtendedTerrainDllDirector()
	{
		std::filesystem::path logFilePath = FileSystem::GetLogFilePath();

		Logger& logger = Logger::GetInstance();
		logger.Init(logFilePath, LogLevel::Error, false);
		logger.WriteLogFileHeader("SC4ExtendedTerrain v" PLUGIN_VERSION_STR);
	}

	uint32_t GetDirectorID() const
	{
		return kExtendedTerrainDirectorID;
	}

	bool PostAppInit()
	{
		extendedTerrainWinManager.PostAppInit();

		return true;
	}

	bool PreAppShutdown()
	{
		extendedTerrainWinManager.PreAppShutdown();

		return true;
	}

	bool OnStart(cIGZCOM* pCOM)
	{
		const uint16_t gameVersion = SC4VersionDetection::GetGameVersion();

		if (gameVersion == 641)
		{
			mpFrameWork->AddHook(this);
		}
		else
		{
			Logger::GetInstance().WriteLineFormatted(
				LogLevel::Error,
				"Unsupported game version %u, version 641 is required.",
				gameVersion);
		}

		return true;
	}

private:
	ExtendedTerrainWinManager extendedTerrainWinManager;
};

cRZCOMDllDirector* RZGetCOMDllDirector() {
	static ExtendedTerrainDllDirector sDirector;
	return &sDirector;
}