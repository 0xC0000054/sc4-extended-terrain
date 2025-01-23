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

enum class TerrainTexture
{
	// SC4 hard-codes this as 0x00000Z11.
	CutawayTop = 0,
	// SC4 hard-codes this as 0x00000Z10.
	CutawayMiddle,
	// SC4 hard-codes this as 0x00000Z15.
	CutawayBottom,
	// SC4 hard-codes this as 0x00000Z14.
	InshoreWater,
	// SC4 hard-codes this as 0x00000Z13.
	// This appears to be used for shallow areas that are on the border
	// of the inshore and outshore textures.
	// It may overlap with the outer edge of the inshore areas.
	MidshoreWater,
	// SC4 hard-codes this as 0x0918730Z.
	// Our redirected textures convert the ID to the 0x00000ZTT format
	// that the rest of the terrain textures use.
	OutshoreWater,
};