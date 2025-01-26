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
#include "TerrainTexture.h"
#include <cstdint>
#include <string>

class TerrainTextureRedirectEntry
{
public:
	TerrainTextureRedirectEntry();
	TerrainTextureRedirectEntry(
		const std::string& terrainName,
		TerrainTexture texture,
		uint32_t redirectedInstanceID);
	TerrainTextureRedirectEntry(
		const std::string& terrainName,
		uint32_t textureIDOffset);

	// We support redirecting requests for some of the game's hard-coded texture instance ids to new
	// values that the user configures in terrain.ini.
	// Note that all instance ids using the redirect system must use the 0x00000ZTT format.
	bool TryGetRedirectedTextureID(TerrainTexture texture, uint32_t& redirectedInstanceID) const;
	void SetRedirectedTextureID(TerrainTexture texture, uint32_t redirectedInstanceID);

	// A texture id offset is used to separate terrain mods that use the same 0x00000ZTT values.
	// The offset is added to the texture id that SC4 requests.
	// For example, if the offset is 0xABCDE000 and the game requests a texture with the instance id
	// 0x00000432; the texture id that is loaded would be 0xABCDE432.
	uint32_t GetTextureIDOffset() const;
	void SetTextureIDOffset(uint32_t offset);

	const std::string& GetTerrainName() const;

	bool operator==(const TerrainTextureRedirectEntry& other) const;

private:
	std::string terrainName;
	uint32_t cutawayTopZoom0TextureID;
	uint32_t cutawayMiddleZoom0TextureID;
	uint32_t cutawayBottomZoom0TextureID;
	uint32_t inshoreWaterZoom0TextureID;
	uint32_t midshoreWaterZoom0TextureID;
	uint32_t outshoreWaterZoom0TextureID;
	uint32_t waterBoxGradientTextureID;
	uint32_t waterColorDepthGradientTextureID;
	uint32_t waterGlareMaskTextureID;
	uint32_t textureIDOffset;
};

