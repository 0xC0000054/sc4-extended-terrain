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

#include "TuningExemplarManager.h"
#include "cGZPersistResourceKey.h"
#include "cIGZPersistResourceManager.h"
#include "GZServPtrs.h"
#include "Logger.h"
#include <cstring>
#include <Windows.h>

namespace
{
	constexpr uint32_t kExemplarType = 0x6534284A;

	constexpr uint32_t kTerrainPropertiesGroup = 0x88CD66E9;
	constexpr uint32_t kTerrainPropertiesInstance = 0x00000001;

	constexpr uint32_t kFloraTuningGroup = 0x7A4A8458;
	constexpr uint32_t kFloraTuningInstance = 0x1A2FDB6A;

	uint32_t ReadUint32(uintptr_t address)
	{
		uint32_t value = 0;
		std::memcpy(&value, reinterpret_cast<const void*>(address), sizeof(value));
		return value;
	}

	bool WriteUint32(uintptr_t address, uint32_t value)
	{
		DWORD oldProtect = 0;

		if (!VirtualProtect(reinterpret_cast<void*>(address), sizeof(value), PAGE_EXECUTE_READWRITE, &oldProtect))
		{
			return false;
		}

		std::memcpy(reinterpret_cast<void*>(address), &value, sizeof(value));

		DWORD unused = 0;
		VirtualProtect(reinterpret_cast<void*>(address), sizeof(value), oldProtect, &unused);
		FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<void*>(address), sizeof(value));

		return true;
	}

	// Checks for 'mov dword ptr [esp+disp8], imm32' (C7 44 24 xx imm32).
	bool IsMovEspDisp8Imm32(uintptr_t instructionAddress)
	{
		const uint8_t* const bytes = reinterpret_cast<const uint8_t*>(instructionAddress);

		return bytes[0] == 0xC7 && bytes[1] == 0x44 && bytes[2] == 0x24;
	}

	bool ResourceExists(uint32_t group, uint32_t instance)
	{
		cIGZPersistResourceManagerPtr pResourceManager;

		if (pResourceManager)
		{
			return pResourceManager->TestForKey(cGZPersistResourceKey(kExemplarType, group, instance));
		}

		return false;
	}
}

TuningExemplarManager::TuningExemplarManager()
	: patchSitesValidated(false)
{
	// The instance immediates of the exemplar readers in SC4 version 641.
	// Each one is preceded by the instruction that stores the group ID.
	patchSites =
	{
		{ 0x74B6CF, kTerrainPropertiesGroup, kTerrainPropertiesInstance, ExemplarKind::TerrainProperties, "cSTETerrain::ReadTuningParameters", false, 0 },
		{ 0x7589BF, kTerrainPropertiesGroup, kTerrainPropertiesInstance, ExemplarKind::TerrainProperties, "cSTETerrainView3D::ReadTuningParameters", false, 0 },
		{ 0x755C7F, kTerrainPropertiesGroup, kTerrainPropertiesInstance, ExemplarKind::TerrainProperties, "cSTETerrainView3D::ReadPlumbingTuningData", false, 0 },
		{ 0x4D8BEF, kTerrainPropertiesGroup, kTerrainPropertiesInstance, ExemplarKind::TerrainProperties, "cSC4ViewInputControlTerrainScrubber::ReadTuningParameters", false, 0 },
		{ 0x6B0122, kFloraTuningGroup, kFloraTuningInstance, ExemplarKind::FloraTuning, "cSC4FloraSimulator::ReadTuning", false, 0 },
	};
}

void TuningExemplarManager::AddExemplarInstance(ExemplarKind kind, const std::string& terrainPrefix, uint32_t instanceID)
{
	if (kind == ExemplarKind::TerrainProperties)
	{
		terrainPropertiesInstances[terrainPrefix] = instanceID;
	}
	else
	{
		floraTuningInstances[terrainPrefix] = instanceID;
	}
}

void TuningExemplarManager::ValidatePatchSites()
{
	Logger& logger = Logger::GetInstance();

	for (PatchSite& site : patchSites)
	{
		const uintptr_t instanceInstruction = site.instanceAddress - 4;
		const uintptr_t groupInstruction = site.instanceAddress - 12;

		site.valid = IsMovEspDisp8Imm32(groupInstruction)
			&& ReadUint32(groupInstruction + 4) == site.group
			&& IsMovEspDisp8Imm32(instanceInstruction)
			&& ReadUint32(site.instanceAddress) == site.originalInstance;

		site.currentInstance = site.originalInstance;

		if (!site.valid)
		{
			logger.WriteLineFormatted(
				LogLevel::Error,
				"%s (0x%08X) does not match the expected code, its exemplar is not switched per terrain type.",
				site.name,
				static_cast<uint32_t>(site.instanceAddress));
		}
	}

	patchSitesValidated = true;
}

uint32_t TuningExemplarManager::SelectInstance(ExemplarKind kind, const std::string& prefix) const
{
	const bool terrainProperties = kind == ExemplarKind::TerrainProperties;
	const auto& instances = terrainProperties ? terrainPropertiesInstances : floraTuningInstances;
	const uint32_t group = terrainProperties ? kTerrainPropertiesGroup : kFloraTuningGroup;
	const uint32_t standardInstance = terrainProperties ? kTerrainPropertiesInstance : kFloraTuningInstance;

	auto item = instances.find(prefix);

	if (item == instances.end())
	{
		return standardInstance;
	}

	if (!ResourceExists(group, item->second))
	{
		Logger::GetInstance().WriteLineFormatted(
			LogLevel::Error,
			"The %s %s exemplar 0x%08X/0x%08X/0x%08X was not found, using the standard exemplar.",
			prefix.c_str(),
			terrainProperties ? "Terrain Properties" : "Flora Tuning",
			kExemplarType,
			group,
			item->second);

		return standardInstance;
	}

	return item->second;
}

void TuningExemplarManager::SetTerrainPrefix(const cRZBaseString& terrainPrefix)
{
	if (terrainPropertiesInstances.empty() && floraTuningInstances.empty())
	{
		// Nothing is configured, the game code is never touched.
		return;
	}

	if (!patchSitesValidated)
	{
		ValidatePatchSites();
	}

	const std::string prefix(terrainPrefix.Data(), terrainPrefix.Strlen());

	const uint32_t instances[2] =
	{
		SelectInstance(ExemplarKind::TerrainProperties, prefix),
		SelectInstance(ExemplarKind::FloraTuning, prefix),
	};

	Logger& logger = Logger::GetInstance();

	for (PatchSite& site : patchSites)
	{
		if (!site.valid)
		{
			continue;
		}

		const uint32_t instance = instances[site.kind == ExemplarKind::TerrainProperties ? 0 : 1];

		if (instance == site.currentInstance)
		{
			continue;
		}

		// Another plugin may have changed the operand since it was last written.
		if (ReadUint32(site.instanceAddress) != site.currentInstance)
		{
			site.valid = false;
			logger.WriteLineFormatted(
				LogLevel::Error,
				"%s (0x%08X) was changed by another plugin, its exemplar is not switched per terrain type.",
				site.name,
				static_cast<uint32_t>(site.instanceAddress));
			continue;
		}

		if (WriteUint32(site.instanceAddress, instance))
		{
			site.currentInstance = instance;
		}
		else
		{
			logger.WriteLineFormatted(
				LogLevel::Error,
				"Failed to patch %s (0x%08X).",
				site.name,
				static_cast<uint32_t>(site.instanceAddress));
		}
	}

	logger.WriteLineFormatted(
		LogLevel::Info,
		"%s: Terrain Properties exemplar 0x%08X, Flora Tuning exemplar 0x%08X.",
		prefix.c_str(),
		instances[0],
		instances[1]);
}
