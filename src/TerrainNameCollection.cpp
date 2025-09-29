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
