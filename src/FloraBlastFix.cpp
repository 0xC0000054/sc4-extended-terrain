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

#include "FloraBlastFix.h"
#include "cISC4FloraSimulator.h"
#include "Logger.h"
#include "cSC4StaticFloraData.h"
#include <array>
#include <cstdint>
#include <cstring>
#include <Windows.h>

namespace
{
	// SC4 version 641 addresses.

	// cSC4FloraSimulator::DropSeed(uint32_t type, float x, float z), thiscall, returns bool.
	constexpr uintptr_t kDropSeedAddress = 0x006B0980;
	// cSC4FloraSimulator::GetPropagationScoreForFlora(uint32_t type, float x, float z), thiscall, returns float.
	constexpr uintptr_t kGetPropagationScoreAddress = 0x006B0900;
	// The 'call DropSeed' instruction in the 'Flora blast' cheat handler.
	constexpr uintptr_t kFloraBlastDropSeedCallAddress = 0x006B32DC;

	typedef bool(__thiscall* DropSeedFn)(void* pThis, uint32_t type, float x, float z);
	typedef float(__thiscall* GetPropagationScoreFn)(void* pThis, uint32_t type, float x, float z);

	const DropSeedFn DropSeed = reinterpret_cast<DropSeedFn>(kDropSeedAddress);
	const GetPropagationScoreFn GetPropagationScoreForFlora = reinterpret_cast<GetPropagationScoreFn>(kGetPropagationScoreAddress);

	struct ExpectedBytes
	{
		uintptr_t address;
		std::array<uint8_t, 5> bytes;
		size_t length;
	};

	// Code that the fix depends on. The record offsets and the score function are checked
	// where DropSeed uses them, so a different layout disables the fix.
	constexpr std::array<ExpectedBytes, 4> kExpectedCode =
	{{
		// DropSeed: call GetPropagationScoreForFlora (0x006B0900).
		{ 0x006B09DD, { 0xE8, 0x1E, 0xFF, 0xFF, 0xFF }, 5 },
		// DropSeed: mov edx, [ebp+0x30] (minimum propagation score).
		{ 0x006B09E6, { 0x8B, 0x55, 0x30, 0x00, 0x00 }, 3 },
		// DropSeed: mov eax, [ebp+0x34] (seed success factor).
		{ 0x006B09F8, { 0x8B, 0x45, 0x34, 0x00, 0x00 }, 3 },
		// Flora blast: call DropSeed (0x006B0980).
		{ kFloraBlastDropSeedCallAddress, { 0xE8, 0x9F, 0xD6, 0xFF, 0xFF }, 5 },
	}};

	// Replaces the cheat's call to DropSeed. __fastcall with an unused edx parameter has the same
	// register and stack layout as a thiscall method with three stack arguments.
	bool __fastcall FloraBlastDropSeed(cISC4FloraSimulator* pThis, void* /*edx*/, uint32_t type, float x, float z)
	{
		if (type != 0)
		{
			const cSC4StaticFloraData* const record = pThis->GetStaticFloraData(type);

			if (record)
			{
				const float seedSuccessFactor = record->parameters[cSC4StaticFloraData::tParameters::SeedSuccessFactor];

				if (!(seedSuccessFactor > 0.0f))
				{
					// The random draw of DropSeed can never pass for this type.
					// Apply the other DropSeed tests and plant the tree.
					if (!pThis->LocationIsOKForNewFlora(type, x, z, nullptr))
					{
						return false;
					}

					const float minPropagationScore = record->parameters[cSC4StaticFloraData::tParameters::MinPropagationScore];
					const float score = GetPropagationScoreForFlora(pThis, type, x, z);

					if (score < minPropagationScore)
					{
						return false;
					}

					// The Y value of 0 makes the game use the ground height, as DropSeed does.
					float position[3] = { x, 0.0f, z };
					pThis->AddNewFloraOccupant(type, position, false);

					return true;
				}
			}
		}

		return DropSeed(pThis, type, x, z);
	}

	bool CodeMatches()
	{
		for (const ExpectedBytes& expected : kExpectedCode)
		{
			if (std::memcmp(reinterpret_cast<const void*>(expected.address), expected.bytes.data(), expected.length) != 0)
			{
				Logger::GetInstance().WriteLineFormatted(
					LogLevel::Error,
					"The Flora blast fix was not installed: the code at 0x%08X does not match the expected code.",
					static_cast<uint32_t>(expected.address));
				return false;
			}
		}

		return true;
	}
}

void FloraBlastFix::Install()
{
	if (!CodeMatches())
	{
		return;
	}

	const uintptr_t callAddress = kFloraBlastDropSeedCallAddress;
	const uintptr_t target = reinterpret_cast<uintptr_t>(&FloraBlastDropSeed);
	const int32_t relativeTarget = static_cast<int32_t>(target - (callAddress + 5));

	void* const operand = reinterpret_cast<void*>(callAddress + 1);
	DWORD oldProtect = 0;

	if (!VirtualProtect(operand, sizeof(relativeTarget), PAGE_EXECUTE_READWRITE, &oldProtect))
	{
		Logger::GetInstance().WriteLine(LogLevel::Error, "The Flora blast fix was not installed: VirtualProtect failed.");
		return;
	}

	std::memcpy(operand, &relativeTarget, sizeof(relativeTarget));

	DWORD unused = 0;
	VirtualProtect(operand, sizeof(relativeTarget), oldProtect, &unused);
	FlushInstructionCache(GetCurrentProcess(), operand, sizeof(relativeTarget));

	Logger::GetInstance().WriteLine(LogLevel::Info, "Installed the Flora blast fix.");
}
