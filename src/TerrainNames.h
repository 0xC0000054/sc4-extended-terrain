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
#include "TerrainNameEntry.h"
#include <vector>

namespace TerrainNames
{
	void Load(std::vector<TerrainNameEntry>& entries);
};

