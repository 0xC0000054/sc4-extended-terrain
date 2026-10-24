/*
 * This file is part of sc4-extended-terrain, a DLL Plugin for SimCity 4
 * that adds support for more terrain types.
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
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class cISC4FloraSimulator;

// Restricts the automatic flora placement (best type search, random seeding,
// the terrain flora brush list and the 'Flora blast' cheat) to the flora types
// of the active terrain type.
//
// terrain.ini maps each terrain prefix to a family ID:
//
// [FloraBiomeFamilies]
// Desert=0x12345678
//
// A flora type is 'managed' when its family list (exemplar property 0xA8F149C5)
// contains one of those family IDs. Managed types that do not belong to the active
// terrain type get the 'disabled for automatic use' flag, the other types keep the
// value from their exemplar. Types without a biome family are never changed.
class FloraBiomeManager
{
public:
	FloraBiomeManager();

	void AddBiomeFamily(const std::string& terrainPrefix, uint32_t familyID);
	void SetTerrainPrefix(const cRZBaseString& terrainPrefix);

	bool HasBiomeFamilies() const;

	// Updates the flora records for the active terrain type.
	// The flora records are static data that is shared by every city in the session,
	// so this must be called again after a city is loaded or the flora exemplars are reloaded.
	void Apply(cISC4FloraSimulator* pFloraSimulator);

private:
	struct OriginalFlags
	{
		const void* record;
		bool disabledForAutomaticUse;
	};

	std::unordered_map<std::string, uint32_t> biomeFamilies;
	std::unordered_set<uint32_t> allBiomeFamilyIDs;
	std::unordered_map<uint32_t, OriginalFlags> originalFlags;
	std::vector<uint32_t> floraTypes;
	uint32_t activeFamilyID;
};
