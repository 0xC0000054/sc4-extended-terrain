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
#include <vector>

// Selects the Terrain Properties and Flora Tuning exemplars of the active terrain type.
//
// terrain.ini maps each terrain prefix to an exemplar instance ID:
//
// [TerrainPropertiesExemplars]
// Desert=0x12345678
//
// [FloraTuningExemplars]
// Desert=0x9ABCDEF0
//
// SC4 loads these exemplars with a fixed TGI. The instance ID is an immediate operand
// of a 'mov dword ptr [esp+disp8], imm32' instruction in each reader, so the instance
// is selected by rewriting those operands. A terrain type without an entry, or an entry
// whose exemplar is not loaded, uses the standard exemplar.
class TuningExemplarManager
{
public:
	enum class ExemplarKind
	{
		TerrainProperties,
		FloraTuning
	};

	TuningExemplarManager();

	void AddExemplarInstance(ExemplarKind kind, const std::string& terrainPrefix, uint32_t instanceID);
	void SetTerrainPrefix(const cRZBaseString& terrainPrefix);

private:
	struct PatchSite
	{
		uintptr_t instanceAddress;
		uint32_t group;
		uint32_t originalInstance;
		ExemplarKind kind;
		const char* name;
		bool valid;
		uint32_t currentInstance;
	};

	void ValidatePatchSites();
	uint32_t SelectInstance(ExemplarKind kind, const std::string& prefix) const;

	std::unordered_map<std::string, uint32_t> terrainPropertiesInstances;
	std::unordered_map<std::string, uint32_t> floraTuningInstances;
	std::vector<PatchSite> patchSites;
	bool patchSitesValidated;
};
