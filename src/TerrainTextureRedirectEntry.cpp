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

#include "TerrainTextureRedirectEntry.h"

TerrainTextureRedirectEntry::TerrainTextureRedirectEntry()
	: terrainName(terrainName),
	  cutawayTopZoom0TextureID(0),
	  cutawayMiddleZoom0TextureID(0),
	  cutawayBottomZoom0TextureID(0),
	  inshoreWaterZoom0TextureID(0),
	  midshoreWaterZoom0TextureID(0),
	  outshoreWaterZoom0TextureID(0),
	  textureIDOffset(0)
{
}

TerrainTextureRedirectEntry::TerrainTextureRedirectEntry(
	const std::string& terrainName,
	TerrainTexture texture,
	uint32_t redirectedInstanceID)
	: terrainName(terrainName),
	  cutawayTopZoom0TextureID(0),
	  cutawayMiddleZoom0TextureID(0),
	  cutawayBottomZoom0TextureID(0),
	  inshoreWaterZoom0TextureID(0),
	  midshoreWaterZoom0TextureID(0),
	  outshoreWaterZoom0TextureID(0),
	  textureIDOffset(0)
{
	SetRedirectedZoom0TextureID(texture, redirectedInstanceID);
}

TerrainTextureRedirectEntry::TerrainTextureRedirectEntry(const std::string& terrainName, uint32_t textureIDOffset)
	: terrainName(terrainName),
	  cutawayTopZoom0TextureID(0),
	  cutawayMiddleZoom0TextureID(0),
	  cutawayBottomZoom0TextureID(0),
	  inshoreWaterZoom0TextureID(0),
	  midshoreWaterZoom0TextureID(0),
	  outshoreWaterZoom0TextureID(0),
	  textureIDOffset(textureIDOffset)
{
}

bool TerrainTextureRedirectEntry::TryGetRedirectedZoom0TextureID(TerrainTexture texture, uint32_t& value) const
{
	switch (texture)
	{
	case TerrainTexture::CutawayTop:
		value = cutawayTopZoom0TextureID;
		break;
	case TerrainTexture::CutawayMiddle:
		value = cutawayMiddleZoom0TextureID;
		break;
	case TerrainTexture::CutawayBottom:
		value = cutawayBottomZoom0TextureID;
		break;
	case TerrainTexture::InshoreWater:
		value = inshoreWaterZoom0TextureID;
		break;
	case TerrainTexture::MidshoreWater:
		value = midshoreWaterZoom0TextureID;
		break;
	case TerrainTexture::OutshoreWater:
		value = outshoreWaterZoom0TextureID;
		break;
	default:
		value = 0;
		break;
	}

	return value != 0;
}

void TerrainTextureRedirectEntry::SetRedirectedZoom0TextureID(TerrainTexture texture, uint32_t value)
{
	switch (texture)
	{
	case TerrainTexture::CutawayTop:
		cutawayTopZoom0TextureID = value;
		break;
	case TerrainTexture::CutawayMiddle:
		cutawayMiddleZoom0TextureID = value;
		break;
	case TerrainTexture::CutawayBottom:
		cutawayBottomZoom0TextureID = value;
		break;
	case TerrainTexture::InshoreWater:
		inshoreWaterZoom0TextureID = value;
		break;
	case TerrainTexture::MidshoreWater:
		midshoreWaterZoom0TextureID = value;
		break;
	case TerrainTexture::OutshoreWater:
		outshoreWaterZoom0TextureID = value;
		break;
	}
}

uint32_t TerrainTextureRedirectEntry::GetTextureIDOffset() const
{
	return textureIDOffset;
}

void TerrainTextureRedirectEntry::SetTextureIDOffset(uint32_t offset)
{
	textureIDOffset = offset;
}

const std::string& TerrainTextureRedirectEntry::GetTerrainName() const
{
	return terrainName;
}

bool TerrainTextureRedirectEntry::operator==(const TerrainTextureRedirectEntry& other) const
{
	return terrainName == other.terrainName;
}
