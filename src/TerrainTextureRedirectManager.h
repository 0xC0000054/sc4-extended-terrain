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

