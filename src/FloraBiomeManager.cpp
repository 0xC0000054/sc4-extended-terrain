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

#include "FloraBiomeManager.h"
#include "cISC4FloraSimulator.h"
#include "cSC4StaticFloraData.h"
#include "Logger.h"

namespace
{
	// A sanity limit for the family list size, a flora exemplar normally lists a few families.
	constexpr size_t kMaxFamilyCount = 256;
}

FloraBiomeManager::FloraBiomeManager()
	: activeFamilyID(0)
{
}

void FloraBiomeManager::AddBiomeFamily(const std::string& terrainPrefix, uint32_t familyID)
{
	if (familyID != 0)
	{
		biomeFamilies[terrainPrefix] = familyID;
		allBiomeFamilyIDs.insert(familyID);
	}
}

void FloraBiomeManager::SetTerrainPrefix(const cRZBaseString& terrainPrefix)
{
	const std::string prefix(terrainPrefix.Data(), terrainPrefix.Strlen());

	auto item = biomeFamilies.find(prefix);

	// A terrain type without a biome family restores the exemplar values of every managed type.
	activeFamilyID = item != biomeFamilies.end() ? item->second : 0;
}

bool FloraBiomeManager::HasBiomeFamilies() const
{
	return !allBiomeFamilyIDs.empty();
}

void FloraBiomeManager::Apply(cISC4FloraSimulator* pFloraSimulator)
{
	if (!pFloraSimulator || allBiomeFamilyIDs.empty())
	{
		return;
	}

	SC4Vector<uint32_t> floraTypes;
	pFloraSimulator->GetAllFloraTypes(floraTypes);

	if (floraTypes.empty())
	{
		return;
	}

	uint32_t managedTypes = 0;
	uint32_t activeTypes = 0;

	for (uint32_t type : floraTypes)
	{
		cSC4StaticFloraData* const record = pFloraSimulator->GetStaticFloraData(type);

		if (!record)
		{
			continue;
		}

		const SC4Vector<uint32_t>& families = record->families;

		if (families.empty() || families.size() > kMaxFamilyCount)
		{
			continue;
		}

		bool managed = false;
		bool inActiveBiome = false;

		for (uint32_t family : families)
		{
			if (allBiomeFamilyIDs.contains(family))
			{
				managed = true;

				if (family == activeFamilyID)
				{
					inActiveBiome = true;
				}
			}
		}

		if (!managed)
		{
			continue;
		}

		uint8_t& flags = record->flags;

		// Remember the exemplar value the first time a record is seen.
		// The game creates new records when it reloads the flora exemplars,
		// so a different record pointer means that the stored value is out of date.
		auto original = originalFlags.find(type);

		if (original == originalFlags.end() || original->second.record != record)
		{
			originalFlags[type] = OriginalFlags{ record, (flags & cSC4StaticFloraData::tFloraFlags::SpecialCaseOnly) != 0 };
			original = originalFlags.find(type);
		}

		const bool disabled = original->second.disabledForAutomaticUse || (activeFamilyID != 0 && !inActiveBiome);

		if (disabled)
		{
			flags |= cSC4StaticFloraData::tFloraFlags::SpecialCaseOnly;
		}
		else
		{
			flags &= ~cSC4StaticFloraData::tFloraFlags::SpecialCaseOnly;
			activeTypes++;
		}

		managedTypes++;
	}

	Logger::GetInstance().WriteLineFormatted(
		LogLevel::Info,
		"Flora biome family 0x%08X: %u of %u managed flora types are enabled for automatic use.",
		activeFamilyID,
		activeTypes,
		managedTypes);
}
