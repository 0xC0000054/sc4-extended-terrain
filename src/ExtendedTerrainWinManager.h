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

#pragma once
#include "cIGZMessageTarget2.h"
#include "IExtendedTerrainWinManager.h"
#include "ExtendedTerrainWin.h"
#include "TerrainNames.h"

class cIGZMessage2Standard;
class cIGZMessageServer2;

class ExtendedTerrainWinManager final :
	private cIGZMessageTarget2,
	private IExtendedTerrainWinManager
{
public:
	ExtendedTerrainWinManager();

	bool QueryInterface(uint32_t riid, void** ppvObj) override;

	uint32_t AddRef() override;

	uint32_t Release() override;

	bool Init();
	bool Shutdown();

private:

	// cIGZMessageTarget2

	bool DoMessage(cIGZMessage2* pMsg) override;

	// IExtendedTerrainWinManager

	void SetCurrentTerrainName(const cIGZWinCombo& combo) override;
	void WindowClosed() override;

	// Private members

	void PreCityInit();
	void PostCityShutdown();
	void PostRegionInit();
	void PreRegionShutdown();
	void ProcessCheatCode(cIGZMessage2Standard* pStandardMsg);

	void ToggleExtendedTerrainWindowVisibility();

	void UpdateTerrainPrefixString(bool saveToRegionFolder);

	uint32_t refCount;
	ExtendedTerrainWin extendedTerrainWin;
	std::vector<TerrainNameEntry> terrainNames;
	cRZBaseString currentTerrainName;
	cIGZWin* pRegionScreen;
	bool initialized;
	bool firstCityLoaded;
	bool exitedCity;
};

