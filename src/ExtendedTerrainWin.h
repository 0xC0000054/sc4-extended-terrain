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
#include "cRZBaseString.h"
#include "cRZBaseWinProc.h"
#include "IExtendedTerrainWinManager.h"
#include "TerrainNameEntry.h"
#include <vector>

class cIGZWin;

class ExtendedTerrainWin final : public cRZBaseWinProc
{
public:
	ExtendedTerrainWin(IExtendedTerrainWinManager& pWinMgr);

	bool Create(
		cIGZWin* const pParentWin,
		const std::vector<TerrainNameEntry>& terrainNames,
		const cRZBaseString& currentTerrainName);

	bool Destroy(cIGZWin& parentWin);

	bool WindowCreated() const;

	void ToggleWindowVisibility();

private:

	bool DoWinProcMessage(cIGZWin*, cGZMessage&) override;

	cIGZWin* pExtendedTerrainWin;
	IExtendedTerrainWinManager& winMgr;
};
