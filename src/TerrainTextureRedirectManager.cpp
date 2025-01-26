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

#include "TerrainTextureRedirectManager.h"
#include "frozen/unordered_map.h"
#include "frozen/unordered_set.h"
#include <algorithm>
#include <limits>

enum class ZoomLevel : uint32_t
{
	Zoom0 = 0,
	Zoom1 = 1,
	Zoom2 = 2,
	Zoom3 = 3,
	Zoom4 = 4
};

struct FixedTerrainTextureInfo
{
	constexpr FixedTerrainTextureInfo(TerrainTexture texture)
		: texture(texture), shiftedZoomLevel(0)
	{
	}

	constexpr FixedTerrainTextureInfo(TerrainTexture texture, ZoomLevel zoomLevel)
		: texture(texture), shiftedZoomLevel(static_cast<uint32_t>(zoomLevel) << 8)
	{
	}

	TerrainTexture texture;
	uint32_t shiftedZoomLevel;
};

// All of these texture instance ids were hard-coded by Maxis.
static constexpr frozen::unordered_map<uint32_t, const FixedTerrainTextureInfo, 30> FixedTerrainTextureIDMap =
{
	{ 0x00000011, FixedTerrainTextureInfo(TerrainTexture::CutawayTop, ZoomLevel::Zoom0) },
	{ 0x00000111, FixedTerrainTextureInfo(TerrainTexture::CutawayTop, ZoomLevel::Zoom1) },
	{ 0x00000211, FixedTerrainTextureInfo(TerrainTexture::CutawayTop, ZoomLevel::Zoom2) },
	{ 0x00000311, FixedTerrainTextureInfo(TerrainTexture::CutawayTop, ZoomLevel::Zoom3) },
	{ 0x00000411, FixedTerrainTextureInfo(TerrainTexture::CutawayTop, ZoomLevel::Zoom4) },

	{ 0x00000010, FixedTerrainTextureInfo(TerrainTexture::CutawayMiddle, ZoomLevel::Zoom0) },
	{ 0x00000110, FixedTerrainTextureInfo(TerrainTexture::CutawayMiddle, ZoomLevel::Zoom1) },
	{ 0x00000210, FixedTerrainTextureInfo(TerrainTexture::CutawayMiddle, ZoomLevel::Zoom2) },
	{ 0x00000310, FixedTerrainTextureInfo(TerrainTexture::CutawayMiddle, ZoomLevel::Zoom3) },
	{ 0x00000410, FixedTerrainTextureInfo(TerrainTexture::CutawayMiddle, ZoomLevel::Zoom4) },

	{ 0x00000015, FixedTerrainTextureInfo(TerrainTexture::CutawayBottom, ZoomLevel::Zoom0) },
	{ 0x00000115, FixedTerrainTextureInfo(TerrainTexture::CutawayBottom, ZoomLevel::Zoom1) },
	{ 0x00000215, FixedTerrainTextureInfo(TerrainTexture::CutawayBottom, ZoomLevel::Zoom2) },
	{ 0x00000315, FixedTerrainTextureInfo(TerrainTexture::CutawayBottom, ZoomLevel::Zoom3) },
	{ 0x00000415, FixedTerrainTextureInfo(TerrainTexture::CutawayBottom, ZoomLevel::Zoom4) },

	{ 0x00000014, FixedTerrainTextureInfo(TerrainTexture::InshoreWater, ZoomLevel::Zoom0) },
	{ 0x00000114, FixedTerrainTextureInfo(TerrainTexture::InshoreWater, ZoomLevel::Zoom1) },
	{ 0x00000214, FixedTerrainTextureInfo(TerrainTexture::InshoreWater, ZoomLevel::Zoom2) },
	{ 0x00000314, FixedTerrainTextureInfo(TerrainTexture::InshoreWater, ZoomLevel::Zoom3) },
	{ 0x00000414, FixedTerrainTextureInfo(TerrainTexture::InshoreWater, ZoomLevel::Zoom4) },

	{ 0x00000013, FixedTerrainTextureInfo(TerrainTexture::MidshoreWater, ZoomLevel::Zoom0) },
	{ 0x00000113, FixedTerrainTextureInfo(TerrainTexture::MidshoreWater, ZoomLevel::Zoom1) },
	{ 0x00000213, FixedTerrainTextureInfo(TerrainTexture::MidshoreWater, ZoomLevel::Zoom2) },
	{ 0x00000313, FixedTerrainTextureInfo(TerrainTexture::MidshoreWater, ZoomLevel::Zoom3) },
	{ 0x00000413, FixedTerrainTextureInfo(TerrainTexture::MidshoreWater, ZoomLevel::Zoom4) },

	{ 0x09187300, FixedTerrainTextureInfo(TerrainTexture::OutshoreWater, ZoomLevel::Zoom0) },
	{ 0x09187301, FixedTerrainTextureInfo(TerrainTexture::OutshoreWater, ZoomLevel::Zoom1) },
	{ 0x09187302, FixedTerrainTextureInfo(TerrainTexture::OutshoreWater, ZoomLevel::Zoom2) },
	{ 0x09187303, FixedTerrainTextureInfo(TerrainTexture::OutshoreWater, ZoomLevel::Zoom3) },
	{ 0x09187304, FixedTerrainTextureInfo(TerrainTexture::OutshoreWater, ZoomLevel::Zoom4) },

	{ 0xC9EFB727, FixedTerrainTextureInfo(TerrainTexture::WaterBoxGradient) },
	{ 0x69EFB6F8, FixedTerrainTextureInfo(TerrainTexture::WaterColorDepthGradient) },
	{ 0x69EFB7DA, FixedTerrainTextureInfo(TerrainTexture::WaterGlareMask) },
};

namespace
{
	uint32_t GetTextureIDWithTerrainOffset(
		const TerrainTextureRedirectEntry& mapping,
		uint32_t originalTextureID)
	{
		// Only apply the texture offset for entries that use the 0x00000ZTT format.
		if ((originalTextureID & 0xfffff000) == 0)
		{
			return mapping.GetTextureIDOffset() + originalTextureID;
		}

		return originalTextureID;
	}

	uint32_t RemapFixedTextureID(
		const TerrainTextureRedirectEntry& mapping,
		const FixedTerrainTextureInfo& info,
		uint32_t originalTextureID)
	{
		uint32_t newTextureIID = 0;

		if (mapping.TryGetRedirectedTextureID(info.texture, newTextureIID))
		{
			if (info.shiftedZoomLevel > 0)
			{
				// Add the correct zoom level to the zoom 0 texture id.
				// We standardize on using the 0x00000014-0x00000414 format for all terrain texture zoom levels.
				newTextureIID = newTextureIID + info.shiftedZoomLevel;
			}
		}
		else
		{
			newTextureIID = originalTextureID;
		}

		return GetTextureIDWithTerrainOffset(mapping, newTextureIID);
	}
}

TerrainTextureRedirectManager::TerrainTextureRedirectManager()
	: currentTerrainMappingIndex(std::numeric_limits<size_t>::max())
{
}

void TerrainTextureRedirectManager::AddTextureRedirect(
	const std::string& terrainName,
	TerrainTexture texture,
	uint32_t redirectedZoom0InstanceID)
{
	auto existingItem = std::find_if(
		terrainTextureMappings.begin(),
		terrainTextureMappings.end(),
		[terrainName](const TerrainTextureRedirectEntry& entry) { return entry.GetTerrainName() == terrainName; });

	if (existingItem != terrainTextureMappings.end())
	{
		existingItem->SetRedirectedTextureID(texture, redirectedZoom0InstanceID);
	}
	else
	{
		terrainTextureMappings.emplace_back(terrainName, texture, redirectedZoom0InstanceID);
	}
}

void TerrainTextureRedirectManager::AddTextureIDOffset(const std::string& terrainName, uint32_t offset)
{
	auto existingItem = std::find_if(
		terrainTextureMappings.begin(),
		terrainTextureMappings.end(),
		[terrainName](const TerrainTextureRedirectEntry& entry) { return entry.GetTerrainName() == terrainName; });

	if (existingItem != terrainTextureMappings.end())
	{
		existingItem->SetTextureIDOffset(offset);
	}
	else
	{
		terrainTextureMappings.emplace_back(terrainName, offset);
	}
}

uint32_t TerrainTextureRedirectManager::GetRedirectedTextureInstanceID(uint32_t originalInstanceID) const
{
	uint32_t result = originalInstanceID;

	if (currentTerrainMappingIndex != std::numeric_limits<size_t>::max())
	{
		const TerrainTextureRedirectEntry& mapping = terrainTextureMappings[currentTerrainMappingIndex];

		const auto fixedTerrainTextureIDItem = FixedTerrainTextureIDMap.find(originalInstanceID);

		if (fixedTerrainTextureIDItem != FixedTerrainTextureIDMap.end())
		{
			result = RemapFixedTextureID(mapping, fixedTerrainTextureIDItem->second, originalInstanceID);
		}
		else
		{
			result = GetTextureIDWithTerrainOffset(mapping, originalInstanceID);
		}
	}

	return result;
}

void TerrainTextureRedirectManager::SetTerrainPrefix(const cRZBaseString& newTerrainPrefix)
{
	currentTerrainMappingIndex = std::numeric_limits<size_t>::max();

	size_t count = terrainTextureMappings.size();

	const std::string_view prefixAsStringView(newTerrainPrefix.Data(), newTerrainPrefix.Strlen());

	for (size_t i = 0; i < count; i++)
	{
		if (terrainTextureMappings[i].GetTerrainName() == prefixAsStringView)
		{
			currentTerrainMappingIndex = i;
			break;
		}
	}
}
