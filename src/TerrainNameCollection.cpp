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

#include "TerrainNameCollection.h"

TerrainNameCollection::TerrainNameCollection()
{
}

TerrainNameCollection::const_iterator TerrainNameCollection::begin() const
{
	return items.begin();
}

TerrainNameCollection::const_iterator TerrainNameCollection::end() const
{
	return items.end();
}

void TerrainNameCollection::clear()
{
	items.clear();
}

void TerrainNameCollection::emplace_back(const std::string& prefix)
{
	items.emplace_back(prefix);
}

void TerrainNameCollection::emplace_back(const std::string& prefix, const std::string& displayName)
{
	items.emplace_back(prefix, displayName);
}

bool TerrainNameCollection::empty() const
{
	return items.empty();
}

TerrainNameCollection::const_iterator TerrainNameCollection::find_display_name(const cRZBaseString& displayName) const
{
	return std::find_if(
		items.begin(),
		items.end(),
		[displayName](const TerrainNameEntry& item) { return displayName.IsEqual(item.GetDisplayName(), true); });
}

TerrainNameCollection::const_iterator TerrainNameCollection::find_prefix_name(const cRZBaseString& prefixName) const
{
	return std::find_if(
		items.begin(),
		items.end(),
		[prefixName](const TerrainNameEntry& item) { return prefixName.IsEqual(item.GetSectionPrefix(), true); });
}

size_t TerrainNameCollection::size() const
{
	return items.size();
}
