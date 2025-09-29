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

#pragma once
#include "cIGZMessageTarget2.h"
#include "IExtendedTerrainWinManager.h"
#include "ITerrainTextureRedirectManager.h"
#include "ExtendedTerrainWin.h"
#include "TerrainNameCollection.h"

class cIGZMessage2Standard;
class cIGZMessageServer2;

class ExtendedTerrainWinManager final :
	private cIGZMessageTarget2,
	private IExtendedTerrainWinManager
{
public:
	ExtendedTerrainWinManager(ITerrainTextureRedirectManager& textureRedirectManager);

	bool QueryInterface(uint32_t riid, void** ppvObj) override;

	uint32_t AddRef() override;

	uint32_t Release() override;

	bool PostAppInit();
	bool PreAppShutdown();

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
	TerrainNameCollection terrainNames;
	cRZBaseString currentTerrainPrefix;
	cIGZWin* pRegionScreen;
	ITerrainTextureRedirectManager& textureRedirectManager;
	bool initialized;
	bool firstCityLoaded;
	bool exitedCity;
};

