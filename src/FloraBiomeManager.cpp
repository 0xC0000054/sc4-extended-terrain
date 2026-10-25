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
#include "Logger.h"

namespace
{
	// Field offsets in the static flora record (cSC4StaticFloraData) of SC4 version 641.
	// cSC4FloraSimulator::AddStaticFloraData copies the exemplar's family list
	// (property 0xA8F149C5) into an STLport vector<uint32_t> at offset 0x154,
	// and keeps the flag bits in the byte at offset 0x16C.
	constexpr size_t kFamilyVectorBeginOffset = 0x154;
	constexpr size_t kFamilyVectorEndOffset = 0x158;
	constexpr size_t kFlagsOffset = 0x16C;

	// Bit 0 of the flags: the type is skipped by the best type search,
	// the random monthly seeding and the terrain flora brush list.
	constexpr uint8_t kDisabledForAutomaticUseFlag = 0x01;

	// A sanity limit for the family list size, a flora exemplar normally lists a few families.
	constexpr ptrdiff_t kMaxFamilyCount = 256;
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

	uint32_t count = 0;
	pFloraSimulator->GetAllFloraTypes(nullptr, count);

	if (count == 0)
	{
		return;
	}

	floraTypes.resize(count);
	count = pFloraSimulator->GetAllFloraTypes(floraTypes.data(), count);
	floraTypes.resize(count);

	uint32_t managedTypes = 0;
	uint32_t activeTypes = 0;

	for (uint32_t type : floraTypes)
	{
		uint8_t* const record = reinterpret_cast<uint8_t*>(pFloraSimulator->GetStaticFloraData(type));

		if (!record)
		{
			continue;
		}

		const uint32_t* const familiesBegin = *reinterpret_cast<uint32_t**>(record + kFamilyVectorBeginOffset);
		const uint32_t* const familiesEnd = *reinterpret_cast<uint32_t**>(record + kFamilyVectorEndOffset);

		if (!familiesBegin || familiesEnd < familiesBegin || (familiesEnd - familiesBegin) > kMaxFamilyCount)
		{
			continue;
		}

		bool managed = false;
		bool inActiveBiome = false;

		for (const uint32_t* family = familiesBegin; family != familiesEnd; ++family)
		{
			if (allBiomeFamilyIDs.contains(*family))
			{
				managed = true;

				if (*family == activeFamilyID)
				{
					inActiveBiome = true;
				}
			}
		}

		if (!managed)
		{
			continue;
		}

		uint8_t& flags = record[kFlagsOffset];

		// Remember the exemplar value the first time a record is seen.
		// The game creates new records when it reloads the flora exemplars,
		// so a different record pointer means that the stored value is out of date.
		auto original = originalFlags.find(type);

		if (original == originalFlags.end() || original->second.record != record)
		{
			originalFlags[type] = OriginalFlags{ record, (flags & kDisabledForAutomaticUseFlag) != 0 };
			original = originalFlags.find(type);
		}

		const bool disabled = original->second.disabledForAutomaticUse || (activeFamilyID != 0 && !inActiveBiome);

		if (disabled)
		{
			flags |= kDisabledForAutomaticUseFlag;
		}
		else
		{
			flags &= static_cast<uint8_t>(~kDisabledForAutomaticUseFlag);
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
