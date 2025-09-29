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
#include "cRZBaseString.h"
#include "cRZBaseWinProc.h"
#include "IExtendedTerrainWinManager.h"
#include "TerrainNameCollection.h"
#include <vector>

class cIGZWin;

class ExtendedTerrainWin final : public cRZBaseWinProc
{
public:
	ExtendedTerrainWin(IExtendedTerrainWinManager& pWinMgr);

	bool Create(
		cIGZWin* const pParentWin,
		const TerrainNameCollection& terrainNames,
		const cRZBaseString& currentTerrainPrefix);

	bool Destroy(cIGZWin& parentWin);

	bool WindowCreated() const;

	void ToggleWindowVisibility();

private:

	bool DoWinProcMessage(cIGZWin*, cGZMessage&) override;

	cIGZWin* pExtendedTerrainWin;
	IExtendedTerrainWinManager& winMgr;
};

