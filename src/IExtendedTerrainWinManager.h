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

#pragma once

class cIGZWinCombo;

class IExtendedTerrainWinManager
{
public:
	virtual void SetCurrentTerrainName(const cIGZWinCombo& combo) = 0;

	virtual void WindowClosed() = 0;
};