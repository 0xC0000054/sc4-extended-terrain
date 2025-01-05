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
#include "cRZAutoRefCount.h"
#include "cRZBaseString.h"
#include "GZServPtrs.h"

#include "ExtendedTerrainWinManager.h"
#include "FileSystem.h"
#include "Logger.h"
#include "SC4VersionDetection.h"
#include "Settings.h"
#include "TerrainS3DTextureBindingFactoryProxy.h"
#include "version.h"

#include <Windows.h>

static constexpr uint32_t kExtendedTerrainDirectorID = 0x7DBE96D9;

namespace
{
	namespace
	{
		bool OverwriteMemoryUint32(uintptr_t address, uint32_t newValue)
		{
			DWORD oldProtect;
			// Allow the executable memory to be written to.
			if (!VirtualProtect(
				reinterpret_cast<void*>(address),
				sizeof(newValue),
				PAGE_EXECUTE_READWRITE,
				&oldProtect))
			{
				return false;
			}

			// Patch the memory at the specified address.
			*((uint32_t*)address) = newValue;
			return true;
		}

		bool InstallTerrainTextureProxyMemoryPatch()
		{
			// Make the cSTETerrainView3D class use our TerrainS3DTextureBindingFactoryProxy service
			// in-place of SC4's built-in cS3DTextureBindingFactory service.
			// This call overwrites the GZCOM service id that the cSTETerrainView3D class will load.
			return OverwriteMemoryUint32(0x75c176, GZSRVID_TerrainS3DTextureBindingFactoryProxy);
		}
	}
}

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

	bool PreFrameWorkInit()
	{
		cRZAutoRefCount<TerrainS3DTextureBindingFactoryProxy> terrainTextureProxy(
			new TerrainS3DTextureBindingFactoryProxy(settings),
			cRZAutoRefCount<TerrainS3DTextureBindingFactoryProxy>::kAddRef);

		cRZAutoRefCount<cIGZSystemService> systemService;

		if (terrainTextureProxy->QueryInterface(GZIID_cIGZSystemService, systemService.AsPPVoid()))
		{
			mpFrameWork->AddSystemService(systemService);
		}
		return true;
	}

	bool PostAppInit()
	{
		cRZAutoRefCount<ITerrainS3DTextureBindingFactoryProxy> terrainTextureProxy;

		if (mpFrameWork->GetSystemService(
			GZSRVID_TerrainS3DTextureBindingFactoryProxy,
			GZIID_ITerrainS3DTextureBindingFactoryProxy,
			terrainTextureProxy.AsPPVoid())
			&& terrainTextureProxy->IsInitialized())
		{
			if (InstallTerrainTextureProxyMemoryPatch())
			{
				extendedTerrainWinManager.PostAppInit();
			}
			else
			{
				Logger::GetInstance().WriteLine(
					LogLevel::Error,
					"Failed to install the cSTETerrainView3D patch.");
			}
		}
		else
		{
			Logger::GetInstance().WriteLine(
				LogLevel::Error,
				"Failed to add the terrain texture loading proxy service.");
		}

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
			settings.Load();
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
	Settings settings;
};

cRZCOMDllDirector* RZGetCOMDllDirector() {
	static ExtendedTerrainDllDirector sDirector;
	return &sDirector;
}