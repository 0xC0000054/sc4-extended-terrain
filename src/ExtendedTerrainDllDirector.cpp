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
#include "TerrainGimexFactoryProxy.h"
#include "TerrainS3DTextureBindingFactoryProxy.h"
#include "TerrainTextureRedirectManager.h"
#include "version.h"

#include <Windows.h>
#include "wil/result.h"

static constexpr uint32_t kExtendedTerrainDirectorID = 0x7DBE96D9;

namespace
{
	namespace
	{
		void OverwriteMemoryUint32(uintptr_t address, uint32_t newValue)
		{
			DWORD oldProtect;
			// Allow the executable memory to be written to.
			THROW_IF_WIN32_BOOL_FALSE(VirtualProtect(
				reinterpret_cast<void*>(address),
				sizeof(newValue),
				PAGE_EXECUTE_READWRITE,
				&oldProtect));

			// Patch the memory at the specified address.
			*((uint32_t*)address) = newValue;
		}
	}
}

class ExtendedTerrainDllDirector : public cRZCOMDllDirector
{
public:
	ExtendedTerrainDllDirector()
		: settings(),
		  textureRedirectManager(),
		  extendedTerrainWinManager(textureRedirectManager)
	{
		std::filesystem::path logFilePath = FileSystem::GetLogFilePath();

		Logger& logger = Logger::GetInstance();
		logger.Init(logFilePath, LogLevel::Error, false);
		logger.WriteLogFileHeader("SC4ExtendedTerrain v" PLUGIN_VERSION_STR);
	}

	bool GetClassObject(uint32_t rclsid, uint32_t riid, void** ppvObj)
	{
		bool result = false;

		if (rclsid == GZCLSID_TerrainGimexFactoryProxy)
		{
			cRZAutoRefCount<TerrainGimexFactoryProxy> proxy(
				new TerrainGimexFactoryProxy(settings, textureRedirectManager),
				cRZAutoRefCount<TerrainGimexFactoryProxy>::kAddRef);

			result = proxy->QueryInterface(riid, ppvObj);
		}

		return result;
	}

	void EnumClassObjects(ClassObjectEnumerationCallback pCallback, void* pContext)
	{
		pCallback(GZCLSID_TerrainGimexFactoryProxy, 0, pContext);
	}

	uint32_t GetDirectorID() const
	{
		return kExtendedTerrainDirectorID;
	}

	bool PreFrameWorkInit()
	{
		cRZAutoRefCount<TerrainS3DTextureBindingFactoryProxy> terrainTextureProxy(
			new TerrainS3DTextureBindingFactoryProxy(settings, textureRedirectManager),
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
			try
			{
				// Patch cSTETerrainView3D to use our TerrainGimexFactoryProxy class in-place
				// of SC4's built-in cGZGimexFactory class.
				// This call overwrites the GZCOM class id that the cSTETerrainView3D class will load.
				OverwriteMemoryUint32(0x75c132, GZCLSID_TerrainGimexFactoryProxy);
				// Patch cSTETerrainView3D to use our TerrainS3DTextureBindingFactoryProxy service
				// in-place of SC4's built-in cS3DTextureBindingFactory service.
				// This call overwrites the GZCOM service id that the cSTETerrainView3D class will load.
				OverwriteMemoryUint32(0x75c176, GZSRVID_TerrainS3DTextureBindingFactoryProxy);
				extendedTerrainWinManager.PostAppInit();
			}
			catch (const std::exception& e)
			{
				Logger::GetInstance().WriteLineFormatted(
					LogLevel::Error,
					"Failed to install the cSTETerrainView3D texture loading patches. %s",
					e.what());
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
	TerrainTextureRedirectManager textureRedirectManager;
};

cRZCOMDllDirector* RZGetCOMDllDirector() {
	static ExtendedTerrainDllDirector sDirector;
	return &sDirector;
}