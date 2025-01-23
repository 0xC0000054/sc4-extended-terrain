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
#include "ITerrainTextureRedirectManager.h"
#include "TerrainTextureRedirectEntry.h"
#include <string>
#include <vector>

class TerrainTextureRedirectManager final : public ITerrainTextureRedirectManager
{
public:
	TerrainTextureRedirectManager();

	void AddTextureRedirect(
		const std::string& terrainName,
		TerrainTexture texture,
		uint32_t redirectedZoom0InstanceID) override;

	void AddTextureIDOffset(const std::string& terrainName, uint32_t offset) override;

	uint32_t GetRedirectedTextureInstanceID(uint32_t originalInstanceID) const override;

	void SetTerrainPrefix(const cRZBaseString& newTerrainPrefix) override;

private:
	size_t currentTerrainMappingIndex;
	std::vector<TerrainTextureRedirectEntry> terrainTextureMappings;
};

