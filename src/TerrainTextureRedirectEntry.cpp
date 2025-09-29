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

#include "TerrainTextureRedirectEntry.h"

TerrainTextureRedirectEntry::TerrainTextureRedirectEntry()
	: terrainName(terrainName),
	  cutawayTopZoom0TextureID(0),
	  cutawayMiddleZoom0TextureID(0),
	  cutawayBottomZoom0TextureID(0),
	  inshoreWaterZoom0TextureID(0),
	  midshoreWaterZoom0TextureID(0),
	  outshoreWaterZoom0TextureID(0),
	  waterBoxGradientTextureID(0),
	  waterColorDepthGradientTextureID(0),
	  waterGlareMaskTextureID(0),
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
	  waterBoxGradientTextureID(0),
	  waterColorDepthGradientTextureID(0),
	  waterGlareMaskTextureID(0),
	  textureIDOffset(0)
{
	SetRedirectedTextureID(texture, redirectedInstanceID);
}

TerrainTextureRedirectEntry::TerrainTextureRedirectEntry(const std::string& terrainName, uint32_t textureIDOffset)
	: terrainName(terrainName),
	  cutawayTopZoom0TextureID(0),
	  cutawayMiddleZoom0TextureID(0),
	  cutawayBottomZoom0TextureID(0),
	  inshoreWaterZoom0TextureID(0),
	  midshoreWaterZoom0TextureID(0),
	  outshoreWaterZoom0TextureID(0),
	  waterBoxGradientTextureID(0),
	  waterColorDepthGradientTextureID(0),
	  waterGlareMaskTextureID(0),
	  textureIDOffset(textureIDOffset)
{
}

bool TerrainTextureRedirectEntry::TryGetRedirectedTextureID(TerrainTexture texture, uint32_t& value) const
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
	case TerrainTexture::WaterBoxGradient:
		value = waterBoxGradientTextureID;
		break;
	case TerrainTexture::WaterColorDepthGradient:
		value = waterColorDepthGradientTextureID;
		break;
	case TerrainTexture::WaterGlareMask:
		value = waterGlareMaskTextureID;
		break;
	default:
		value = 0;
		break;
	}

	return value != 0;
}

void TerrainTextureRedirectEntry::SetRedirectedTextureID(TerrainTexture texture, uint32_t value)
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
	case TerrainTexture::WaterBoxGradient:
		waterBoxGradientTextureID = value;
		break;
	case TerrainTexture::WaterColorDepthGradient:
		waterColorDepthGradientTextureID = value;
		break;
	case TerrainTexture::WaterGlareMask:
		waterBoxGradientTextureID = value;
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
