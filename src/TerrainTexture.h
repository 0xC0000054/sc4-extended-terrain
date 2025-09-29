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
	// SC4 hard-codes this as 0xC9EFB727, there are no zoom levels.
	// Our redirected textures convert the ID to the 0x000000TT format
	// that the rest of the terrain textures use.
	WaterBoxGradient,
	// SC4 hard-codes this as 0x69EFB6F8, there are no zoom levels.
	// Our redirected textures convert the ID to the 0x000000TT format
	// that the rest of the terrain textures use.
	WaterColorDepthGradient,
	// SC4 hard-codes this as 0x69EFB7DA, there are no zoom levels.
	// Our redirected textures convert the ID to the 0x000000TT format
	// that the rest of the terrain textures use.
	WaterGlareMask,
};