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
#include "SC4NotificationDialog.h"

#include "ExtendedTerrainWinManager.h"
#include "FileSystem.h"
#include "Logger.h"
#include "TerrainNames.h"
#include "version.h"

#include <array>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <Windows.h>
#include "wil/resource.h"
#include "wil/win32_helpers.h"

#include "EASTLConfigSC4.h"
#include "EASTL\vector.h"

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
		extendedTerrainWinManager.Init();

		return true;
	}

	bool PreAppShutdown()
	{
		extendedTerrainWinManager.Shutdown();

		return true;
	}

	bool OnStart(cIGZCOM * pCOM)
	{
		const cIGZFrameWork::FrameworkState state = mpFrameWork->GetState();

		if (state < cIGZFrameWork::kStatePreAppInit)
		{
			mpFrameWork->AddHook(this);
		}
		else
		{
			PreAppInit();
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