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
