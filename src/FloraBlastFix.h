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

// Lets the 'Flora blast' cheat plant flora types whose seed success factor is 0.
//
// The cheat plants each tree through cSC4FloraSimulator::DropSeed, which accepts a seed only when
// (score - minScore) * random(0..1) < (1 - minScore) * seedSuccessFactor.
// Flora types that disable natural seeding with a seed success factor of 0 (for example
// seasonal tree controllers) can never pass that test, so the cheat plants nothing.
//
// The fix redirects the cheat's DropSeed call. For a type with a seed success factor of 0 or less,
// it performs the same placement and propagation score tests without the random draw.
// Every other type, and natural seeding, use the game's DropSeed unchanged.
namespace FloraBlastFix
{
	void Install();
}
