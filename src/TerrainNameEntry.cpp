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

#include "TerrainNameEntry.h"

TerrainNameEntry::TerrainNameEntry()
	: displayName(), sectionPrefix()
{
}

TerrainNameEntry::TerrainNameEntry(const std::string& prefix)
	: displayName(), sectionPrefix(prefix)
{
}

TerrainNameEntry::TerrainNameEntry(
	const std::string& prefix,
	const std::string& displayName)
	: displayName(displayName), sectionPrefix(prefix)
{
}

const cRZBaseString& TerrainNameEntry::GetDisplayName() const
{
	return displayName.Strlen() > 0 ? displayName : sectionPrefix;
}

const cRZBaseString& TerrainNameEntry::GetSectionPrefix() const
{
	return sectionPrefix;
}
