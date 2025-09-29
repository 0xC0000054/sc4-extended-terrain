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
#include "cRZBaseString.h"
#include "TerrainTexture.h"
#include <string>

class ITerrainTextureRedirectManager
{
public:
	virtual void AddTextureRedirect(
		const std::string& terrainName,
		TerrainTexture texture,
		uint32_t redirectedZoom0InstanceID) = 0;

	virtual void AddTextureIDOffset(const std::string& terrainName,	uint32_t offset) = 0;

	virtual uint32_t GetRedirectedTextureInstanceID(uint32_t originalInstanceID) const = 0;

	virtual void SetTerrainPrefix(const cRZBaseString& newTerrainPrefix) = 0;
};
