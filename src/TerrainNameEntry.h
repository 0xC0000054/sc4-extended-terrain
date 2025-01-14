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
#include "cRZBaseString.h"

class TerrainNameEntry
{
public:
	TerrainNameEntry();
	TerrainNameEntry(const std::string& prefix);
	TerrainNameEntry(const std::string& prefix, const std::string& displayName);

	const cRZBaseString& GetDisplayName() const;
	const cRZBaseString& GetSectionPrefix() const;

private:
	cRZBaseString displayName;
	cRZBaseString sectionPrefix;
};