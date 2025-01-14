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
#include <string>
#include <vector>

class TerrainNameCollection
{
public:
	using collection_type = std::vector<TerrainNameEntry>;
	using const_iterator = collection_type::const_iterator;

	TerrainNameCollection();

	const_iterator begin() const;
	const_iterator end() const;

	void clear();

	void emplace_back(const std::string& prefix);
	void emplace_back(const std::string& prefix, const std::string& displayName);

	bool empty() const;

	const_iterator find_display_name(const cRZBaseString& displayName) const;
	const_iterator find_prefix_name(const cRZBaseString& prefixName) const;

	size_t size() const;

private:
	collection_type items;
};

