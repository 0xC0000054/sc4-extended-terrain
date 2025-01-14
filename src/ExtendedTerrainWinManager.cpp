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

#include "ExtendedTerrainWinManager.h"
#include "FileSystem.h"
#include "Logger.h"
#include "cGZMessage.h"
#include "cGZPersistResourceKey.h"
#include "cIGZCheatCodeManager.h"
#include "cIGZMessage2Standard.h"
#include "cIGZMessageServer2.h"
#include "cIGZPersistResourceManager.h"
#include "cIGZWin.h"
#include "cIGZWinCombo.h"
#include "cISC4App.h"
#include "cRZAutoRefCount.h"
#include "GZCLSIDDefs.h"
#include "GZServPtrs.h"
#include "RegionTerrainData.h"
#include "SC4String.h"
#include "TerrainIniReader.h"

#include <array>
#include <functional>
#include <string>
#include <vector>

static constexpr uint32_t kSC4MessagePreCityInit = 0x26D31EC0;
static constexpr uint32_t kSC4MessagePostCityShutdown = 0x26D31EC3;
static constexpr uint32_t kSC4MessagePostRegionInit = 0xCBB5BB45;
static constexpr uint32_t kSC4MessagePreRegionShutdown = 0x8BB5BB46;
static constexpr uint32_t kMessageCheatIssued = 0x230E27AC;

static constexpr uint32_t kToggleExtendedTerrainDialogCheatID = 0xDD03DB81;

static const uint32_t kGZWin_WinSC4App = 0x6104489A;
static const uint32_t kGZWin_SC4WinRegionScreen = 0xEA659793;

static constexpr std::array<uint32_t, 3> RequiredNotifications =
{
	kSC4MessagePostCityShutdown,
	kSC4MessagePostRegionInit,
	kSC4MessagePreRegionShutdown
};

// The prefix string SC4 uses to decide which terrain textures to load.
// The default value is 'Tropical', which is the only terrain type Maxis implemented.
static SC4String* const spTerrainTexturePrefix = reinterpret_cast<SC4String*>(0xb4bc64);

typedef bool(__cdecl* cSTETerrain_StaticShutdownFn)(void*);

static const cSTETerrain_StaticShutdownFn cSTETerrain_StaticShutdown = reinterpret_cast<cSTETerrain_StaticShutdownFn>(0x74fc20);


ExtendedTerrainWinManager::ExtendedTerrainWinManager()
	: refCount(0),
	  extendedTerrainWin(*this),
	  pRegionScreen(nullptr),
	  initialized(false),
	  firstCityLoaded(false),
	  exitedCity(false)
{
}

bool ExtendedTerrainWinManager::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZCLSID::kcIGZMessageTarget2)
	{
		*ppvObj = static_cast<cIGZMessageTarget2*>(this);

		AddRef();

		return true;
	}
	else if (riid == GZIID_cIGZUnknown)
	{
		*ppvObj = static_cast<cIGZUnknown*>(static_cast<cIGZMessageTarget2*>(this));

		AddRef();

		return true;
	}

	return false;
}

uint32_t ExtendedTerrainWinManager::AddRef()
{
	return ++refCount;
}

uint32_t ExtendedTerrainWinManager::Release()
{
	if (refCount > 0)
	{
		--refCount;
	}

	return refCount;
}

bool ExtendedTerrainWinManager::PostAppInit()
{
	if (!initialized)
	{
		TerrainIniReader::Parse(terrainNames);

		cIGZMessageServer2Ptr pMS2;

		if (pMS2)
		{
			for (uint32_t messageID : RequiredNotifications)
			{
				pMS2->AddNotification(this, messageID);
			}

			// PreCityInit is registered separately because it
			// will be unregistered when the game calls it.
			pMS2->AddNotification(this, kSC4MessagePreCityInit);
		}

		initialized = true;
	}

	return true;
}

bool ExtendedTerrainWinManager::PreAppShutdown()
{
	if (initialized)
	{
		cIGZMessageServer2Ptr pMS2;

		if (pMS2)
		{
			for (uint32_t messageID : RequiredNotifications)
			{
				pMS2->RemoveNotification(this, messageID);
			}
		}

		initialized = false;
	}

	return true;
}

bool ExtendedTerrainWinManager::DoMessage(cIGZMessage2* pMessage)
{
	cIGZMessage2Standard* pStandardMsg = static_cast<cIGZMessage2Standard*>(pMessage);
	uint32_t dwType = pStandardMsg->GetType();

	switch (dwType)
	{
	case kSC4MessagePreCityInit:
		PreCityInit();
		break;
	case kSC4MessagePostCityShutdown:
		PostCityShutdown();
		break;
	case kSC4MessagePostRegionInit:
		PostRegionInit();
		break;
	case kSC4MessagePreRegionShutdown:
		PreRegionShutdown();
		break;
	case kMessageCheatIssued:
		ProcessCheatCode(pStandardMsg);
		break;
	}

	return true;
}

void ExtendedTerrainWinManager::SetCurrentTerrainName(const cIGZWinCombo& combo)
{
	cRZBaseString displayName;

	if (combo.GetStringFromIndex(combo.GetSelection(), displayName))
	{
		auto iterator = terrainNames.find_display_name(displayName);

		if (iterator != terrainNames.end())
		{
			currentTerrainPrefix = iterator->GetSectionPrefix();
		}
	}
}

void ExtendedTerrainWinManager::WindowClosed()
{
	UpdateTerrainPrefixString(true);
}

void ExtendedTerrainWinManager::PreCityInit()
{
	firstCityLoaded = true;

	cIGZMessageServer2Ptr pMS2;

	if (pMS2)
	{
		pMS2->RemoveNotification(this, kSC4MessagePreCityInit);
	}
}

void ExtendedTerrainWinManager::PostCityShutdown()
{
	exitedCity = true;
}

void ExtendedTerrainWinManager::PostRegionInit()
{
	Logger& logger = Logger::GetInstance();

	cISC4AppPtr pSC4App;

	if (pSC4App)
	{
		cIGZWin* mainWindow = pSC4App->GetMainWindow();

		if (mainWindow)
		{
			cIGZWin* pWinSC4App = mainWindow->GetChildWindowFromID(kGZWin_WinSC4App);

			if (pWinSC4App)
			{
				pRegionScreen = pWinSC4App->GetChildWindowFromID(kGZWin_SC4WinRegionScreen);

				if (!pRegionScreen)
				{
					logger.WriteLine(LogLevel::Error, "The region screen window pointer was null.");
					return;
				}

				// We increment the reference count on the window we get from GetChildWindowFromID
				// because this window reference to perform clean up when a region shutdown
				// message is received.
				pRegionScreen->AddRef();
			}
			else
			{
				logger.WriteLine(LogLevel::Error, "The SC4App window pointer was null.");
				return;
			}
		}
		else
		{
			logger.WriteLine(LogLevel::Error, "The main window pointer was null.");
			return;
		}

		cIGZCheatCodeManager* pCheatMgr = pSC4App->GetCheatCodeManager();

		if (pCheatMgr)
		{
			pCheatMgr->AddNotification2(this, 0);
			pCheatMgr->RegisterCheatCode(kToggleExtendedTerrainDialogCheatID, cRZBaseString("ExtendedTerrain"));
		}
		else
		{
			logger.WriteLine(LogLevel::Error, "The cheat manager pointer was null.");
		}

		if (exitedCity)
		{
			exitedCity = false;
		}
		else
		{
			if (!RegionTerrainData::Load(*pSC4App, currentTerrainPrefix))
			{
				// If the region data doesn't exist, use SC4's standard terrain type.
				currentTerrainPrefix.FromChar("Tropical");
			}

			UpdateTerrainPrefixString(false);
		}
	}
	else
	{
		logger.WriteLine(LogLevel::Error, "The SC4 application pointer was null.");
	}
}

void ExtendedTerrainWinManager::PreRegionShutdown()
{
	cISC4AppPtr pSC4App;

	if (pSC4App)
	{
		cIGZCheatCodeManager* pCheatMgr = pSC4App->GetCheatCodeManager();

		if (pCheatMgr)
		{
			pCheatMgr->RemoveNotification2(this, 0);
			pCheatMgr->UnregisterCheatCode(kToggleExtendedTerrainDialogCheatID);
		}
	}

	if (pRegionScreen)
	{
		extendedTerrainWin.Destroy(*pRegionScreen);

		pRegionScreen->Release();
		pRegionScreen = nullptr;
	}
}

void ExtendedTerrainWinManager::ProcessCheatCode(cIGZMessage2Standard* pStandardMsg)
{
	uint32_t cheatID = static_cast<uint32_t>(pStandardMsg->GetData1());

	if (cheatID == kToggleExtendedTerrainDialogCheatID)
	{
		ToggleExtendedTerrainWindowVisibility();
	}
}

void ExtendedTerrainWinManager::ToggleExtendedTerrainWindowVisibility()
{
	if (extendedTerrainWin.WindowCreated())
	{
		extendedTerrainWin.ToggleWindowVisibility();
	}
	else
	{
		extendedTerrainWin.Create(pRegionScreen, terrainNames, currentTerrainPrefix);
	}
}

void ExtendedTerrainWinManager::UpdateTerrainPrefixString(bool saveToRegionFolder)
{
	cIGZString* const texturePrefixAsGZString = spTerrainTexturePrefix->AsIGZString();

	if (currentTerrainPrefix.Strlen() > 0
		&& !texturePrefixAsGZString->IsEqual(currentTerrainPrefix, false))
	{
		texturePrefixAsGZString->Copy(currentTerrainPrefix);

		if (firstCityLoaded)
		{
			// Call the cSTETerrain::StaticShutdown function to make it think SC4 has restarted.
			// This makes the game parse terrain.ini again.
			cSTETerrain_StaticShutdown(nullptr);
		}

		if (saveToRegionFolder)
		{
			cISC4AppPtr pSC4App;

			if (pSC4App)
			{
				if (!RegionTerrainData::Save(*pSC4App, currentTerrainPrefix))
				{
					Logger::GetInstance().WriteLine(
						LogLevel::Error,
						"Failed to save the region terrain data.");
				}
			}
		}
	}
}
