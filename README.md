# sc4-extended-terrain

A DLL Plugin for SimCity 4 that adds support for more terrain types.

Based on the data in terrain.ini, Maxis appears to have originally intended for SC4 to have 4 different terrain types:
Arctic, Desert, Temperate and Tropical. That feature was cut during the game's development, and only the Tropical
terrain type was implemented.

This plugin adds a region view cheat code that displays a UI for switching between terrain types.
The available terrain types are shown using the prefix that the terrain mod specifies in terrain.ini.
The prefix is the text that comes before the _TextureMapTable_ and _MiscTextures_ section names.
_Terrain_ is not a valid terrain mod prefix: _TerrainMiscTextures_ is the section that the game uses for the cliff
textures shared between terrain mods.

The plugin is intended as a dependency for mods that add new terrain types beyond the Maxis Tropical terrain type.
It does not add any terrain types to the game by itself.

## Features

* Switching the terrain type of a region from the region view, including after a city has been loaded.
* Display names for the terrain types.
* Overrides for the hard-coded water, cutaway and glare textures.
* Texture offsets, so several terrain mods can use the same Maxis texture IDs.
* Per-terrain flora: each terrain type can use its own tree controller.
* Per-terrain Terrain Properties and Flora Tuning exemplars.
* _Flora blast_ cheat support for flora types that have natural seeding disabled, such as seasonal trees.

When the terrain type changes, the terrain and terrain view data are reset and rebuilt for the new terrain type
when the next city is loaded.

## Terrain.ini Reference

The plugin adds the following functionality to the Maxis terrain.ini record.

### DisplayNames Section

Provides an optional display name for each terrain type in the Extended Terrain UI.
The key is the terrain prefix, and the value is the display name.
Terrain types that are not listed use the prefix as the display name.

```ini
[DisplayNames]
Desert=Sahara Desert
```

### Extended MiscTextures Values

The following hard-coded texture IDs can be overridden in the terrain type's MiscTextures section.
All override texture IDs use the 0x000000TT format, like the other terrain.ini textures.

| Field Name | Maxis Texture ID | Description |
|------------|------------------|-------------|
| CutawayTop | 0x00000Z11 | The top layers of the city terrain cutaway. |
| CutawayMiddle | 0x00000Z10 | The middle layers of the city terrain cutaway. |
| CutawayBottom | 0x00000Z15 | The bottom layers of the city terrain cutaway. |
| InshoreWater | 0x00000Z14 | The inshore water texture. |
| MidshoreWater | 0x00000Z13 | The midshore water texture. |
| OutshoreWater | 0x0918730Z | The outshore water texture. |
| WaterBoxGradient | 0xC9EFB727 | The outshore cutaway water gradient texture. Does not have zoom levels. |
| WaterColorDepthGradient | 0x69EFB6F8 | The inshore cutaway water gradient texture. Does not have zoom levels. |
| WaterGlareMask | 0x69EFB7DA | A grayscale mask used for the sun reflections on the water. Does not have zoom levels. |

```ini
[TropicalMiscTextures]
HighCliff=0x00000043
LowCliff=0x00000044
Beach=0x00000039
OutshoreWater=0x000000FF
WaterGlareMask=0x000000FE
```

### TextureOffsets Section

Allows several terrain mods to use the same Maxis texture IDs by applying an offset.
For example, if the offset is 0xABCDE000 and the game requests the texture 0x00000432,
the texture that is loaded is 0xABCDE432.

Maxis terrain textures use the following format:

```
0x00000ZTT

Z  - the zoom level (0-4).
TT - the texture id.
```

The upper 20 bits of the Maxis IDs are always 0, so they are used as an offset to separate terrain textures
that share the same ZTT values. The Texture Index ID is used for the upper 16 bits of the offset.

```
0xABCDEZTT

ABCD - the Texture Index id.
E    - reserved.
Z    - the zoom level (0-4).
TT   - the texture id.
```

The key is the terrain prefix, and the value is the offset for that terrain type.

```ini
[TextureOffsets]
Desert=0x12345
```

#### Reserved Texture Offsets

The following offsets are reserved to avoid conflicts with the game's own texture IDs in the
0x7AB50E44, 0x891B0E1A group.

<details>
<summary>Reserved Texture Offsets</summary>

```
0x08F65
0x09187
0x095A8
0x09909
0x0999D
0x0A051
0x0A0B4
0x28EB8
0x28F7A
0x292C4
0x2A05B
0x4A052
0x695C0
0x699CA
0x69EFB
0x69F00
0x69F10
0x69F40
0x69F50
0x69F80
0x69F90
0x69FA0
0x69FB0
0x6A05B
0x6A6C9
0x6A849
0x88F7A
0x8A050
0xA9860
0xAA050
0xC9909
0xC9EFB
0xCA050
0xCA131
0xCA5CC
0xE8EA9
0xE8F79
0xE92C4
0xE95A8
0xE9860
0xE9A44
0xEA05D
```
</details>

### FloraBiomeFamilies Section

Assigns a flora family ID to each terrain type, so each terrain type can use its own tree controller.
The key is the terrain prefix, and the value is the family ID.

```ini
[FloraBiomeFamilies]
Desert=0xF18AA3A7
Temperate=0x6B9D4447
Arctic=0x5C6459ED
Tropical=0xDAB353D8
```

A flora type belongs to a terrain type when the family ID is in the family list (property 0xA8F149C5)
of its exemplar. A flora type can belong to several terrain types.

| Flora type | Automatic placement |
|------------|---------------------|
| In the family of the active terrain type | Uses the value from its exemplar. |
| Only in the families of other terrain types | Disabled. |
| Not in any of the listed families (for example the Maxis trees) | Unchanged. |

Automatic placement covers the best type search, the terrain tool flora brush, the random monthly seeding and the
_Flora blast_ cheat. The manual flora brush and lots can plant any flora type, and existing flora is not changed.
The settings are applied when a city is loaded and when the flora exemplars are reloaded.

### TerrainPropertiesExemplars and FloraTuningExemplars Sections

Select a Terrain Properties and a Flora Tuning exemplar for each terrain type.
The key is the terrain prefix, and the value is the instance ID of the exemplar.
The exemplar uses the same type and group as the standard exemplar.

| Section | Type | Group | Standard Instance |
|---------|------|-------|-------------------|
| TerrainPropertiesExemplars | 0x6534284A | 0x88CD66E9 | 0x00000001 |
| FloraTuningExemplars | 0x6534284A | 0x7A4A8458 | 0x1A2FDB6A |

```ini
[TerrainPropertiesExemplars]
Desert=0x82FF1151
Arctic=0x40E82E9D

[FloraTuningExemplars]
Desert=0x77004BA5
Arctic=0x14D74D52
```

* A terrain type without an entry, or with an entry whose exemplar is not loaded, uses the standard exemplar.
* The Terrain Properties exemplar is applied when a city is loaded after the terrain type was changed.
The Flora Tuning exemplar is applied every time a city is loaded.
* The water ripple (instance 0x00000002) and volcano lava (instance 0x00000005) exemplars always use the
standard instances.
* To create a terrain type's exemplar, copy the standard exemplar, give it a new instance ID and edit the
properties that should differ for that terrain type.

The game loads these exemplars with fixed instance IDs. The plugin selects the terrain type's exemplar by changing
the instance ID used by the following functions. Each address is verified before it is changed: the instruction,
the group ID and the original instance ID must match the expected values, otherwise the address is left
unchanged and an error is written to the log.

| Address | Function | Exemplar |
|---------|----------|----------|
| 0x0074B6CF | cSTETerrain::ReadTuningParameters | Terrain Properties |
| 0x007589BF | cSTETerrainView3D::ReadTuningParameters | Terrain Properties |
| 0x00755C7F | cSTETerrainView3D::ReadPlumbingTuningData | Terrain Properties |
| 0x004D8BEF | cSC4ViewInputControlTerrainScrubber::ReadTuningParameters | Terrain Properties |
| 0x005E3518 | Function at 0x005E3400 | Terrain Properties |
| 0x006B0122 | cSC4FloraSimulator::ReadTuning | Flora Tuning |

## Flora Blast Cheat

The _Flora blast_ cheat plants each tree through the game's seed routine, which accepts a seed only when
`(score - minScore) * random(0..1) < (1 - minScore) * seedSuccessFactor`, where `minScore` and `seedSuccessFactor`
are elements 8 and 9 of the flora exemplar property 0x083DC35B. Flora types that disable natural seeding with a
seed success factor of 0, such as seasonal tree controllers, can never pass this test.

For flora types with a seed success factor of 0 or less, the plugin makes the cheat apply the placement and
minimum propagation score tests without the random draw. Other flora types, and natural seeding, are not changed.

## ExtendedTerrain INI

_SC4ExtendedTerrain.ini_ contains two options for logging the game's terrain texture loading.

| Option | Description |
|--------|-------------|
| LogTextureIDs | Logs the texture IDs as the game requests them. |
| LogTextureLoadErrors | Logs only the texture IDs that the game fails to load. |

## System Requirements

* SimCity 4 version 641
* Windows 10 or later

The plugin may work on Windows 7 or later with the
[Microsoft Visual C++ 2022 x86 Redistributable](https://aka.ms/vs/17/release/vc_redist.x86.exe) installed,
but this has not been tested.

## Installation

1. Close SimCity 4.
2. Copy _SC4ExtendedTerrain.dll_, _SC4ExtendedTerrain.ini_ and _ExtendedTerrainUI.dat_ into the top level of the
Plugins folder in the SimCity 4 installation directory or in the Documents/SimCity 4 directory.
3. Start SimCity 4.

## Usage

In the region view, enter the _ExtendedTerrain_ cheat code. It has no parameters and shows a dialog that sets the
terrain type of the region. The new terrain type is used by the next city that is loaded.

## Troubleshooting

The plugin writes a `SC4ExtendedTerrain.log` file in the same folder as the plugin.
The log contains status information for the most recent run of the plugin, including:

* the Terrain Properties and Flora Tuning exemplars used by the active terrain type;
* the number of flora types enabled for automatic placement;
* any game code that does not match the expected version.

## Known Limitations

* Textures of a previously active terrain type stay in memory until the game is restarted.
* Only SimCity 4 version 641 is supported.

# License

This project is licensed under the terms of the GNU Lesser General Public License version 3.0.
See [LICENSE.txt](LICENSE.txt) for more information.

## 3rd party code

[gzcom-dll](https://github.com/nsgomez/gzcom-dll/tree/master) - LGPL 2.1 or later License.
[Windows Implementation Library](https://github.com/microsoft/wil) - MIT License.
[SC4Fix](https://github.com/nsgomez/sc4fix) - MIT License.
[Frozen](https://github.com/serge-sans-paille/frozen) - Apache 2.0 License.
[Boost.Algorithm](https://www.boost.org/doc/libs/1_84_0/libs/algorithm/doc/html/index.html) - Boost Software License, Version 1.0.
[Boost.Endian](https://www.boost.org/doc/libs/1_84_0/libs/endian/doc/html/endian.html) - Boost Software License, Version 1.0.
[Boost.PropertyTree](https://www.boost.org/doc/libs/1_84_0/doc/html/property_tree.html) - Boost Software License, Version 1.0.

# Source Code

## Prerequisites

* Visual Studio 2022 with the "Desktop development with C++" workload
* `git submodule update --init`
* [vcpkg](https://github.com/microsoft/vcpkg) with the Visual Studio integration

The plugin must be built with the Microsoft C++ compiler (MSVC). The game interfaces that the plugin implements
have overloaded virtual methods, and their virtual table layout depends on the MSVC ABI.

## Building the plugin

* Open the solution in the `src` folder.
* Update the post build events to copy the build output to your SimCity 4 Plugins folder.
* Build the solution.

To build from the command line without the post build event:

```
msbuild src\SC4ExtendedTerrain.sln /m /p:Configuration=Release /p:Platform=x86 /p:PostBuildEventUseInBuild=false
```

## Debugging the plugin

Visual Studio can be configured to launch SimCity 4 on the Debugging page of the project properties.
The following command line launches the game in a window:
`-intro:off -CPUcount:1 -w -CustomResolution:enabled -r1920x1080x32`

Adjust the resolution for your screen.
