# sc4-extended-terrain

A DLL Plugin for SimCity 4 that adds support for more terrain types.   

Based on the data in terrain.ini, Maxis appears to have originally intended for SC4 to have 4 different terrain types: Arctic, Desert, Temperate and Tropical.
But that feature was cut at some point in the game's development, and they only implemented the Tropical terrain type.

This DLL mod adds a region view cheat code that displays a UI for the user to switch between the different terrain types.
The available terrain types will be shown using the prefix the terrain mod specifies in terrain.ini.
The prefix is the text that comes before the _TextureMapTable_ and _MiscTexture_ section names.
Note that the _Terrain_ not a valid terrain mod prefix,  _TerrainMiscTextures_ is the section the game uses for cliff textures that
are shared between different texture mods.

This DLL mod is intended as a dependency for other mods that add new terrain types beyond the Tropical
terrain type that Maxis created, it does not add any terrain types to the game by itself.

## New Terrain.ini Functionality

The plugin add the following new functionality to the Maxis terrain.ini record.

### DisplayNames Section

This section allows terrain mod developers to optionally provide a display name for use in the Extended Terrain UI.
The section entry key is the prefix name, and the value is the display name.
Any terrain types that are not included will use the prefix name as the display name.

```ini
[DisplayNames]
Desert=Sahara Desert
```

### Extended MiscTextures Values

The following hard-coded texture ids can now be overridden in terrain.ini.
Note that all override texture ids must use the 0x000000TT id format, like the other terrain.ini textures.

| Field Name | Maxis Texture ID | Description |
|-----------|------------------|-------------|
| CutawayTop | 0x00000Z11 | The top layers of the city terrain cutaway. |
| CutawayMiddle | 0x00000Z10 | The middle layers of the city terrain cutaway. |
| CutawayBottom | 0x00000Z15 | The bottom layers of the city terrain cutaway. |
| InshoreWater | 0x00000Z14 | The inshore water texture. |
| MidshoreWater | 0x00000Z13 | The midshore water texture. |
| OutshoreWater | 0x0918730Z | The outshore water texture. |
| WaterBoxGradient | 0xC9EFB727 | The outshore cutaway water gradient texture. Does not have different zoom levels. |
| WaterColorDepthGradient | 0x69EFB6F8 | The inshore cutaway water gradient texture. Does not have different zoom levels |
| WaterGlareMask | 0x69EFB7DA | A gray scale mask that is used for the sun reflections on the water. Does not have different zoom levels |

#### Example INI Section

```ini
[TropicalMiscTextures]
HighCliff=0x00000043
LowCliff=0x00000044
Beach=0x00000039
OutshoreWater=0x000000FF
```

### TextureOffsets Section

This section allows multiple terrain textures to use the same Maxis texture ids by using an offset system.    
For example, if the offset is 0xABCDE000 and the game requests a texture with the instance id 0x00000432;
the texture id that is loaded would be 0xABCDE432.

#### Format Description

Maxis used the following terrain texture format.

```
0x00000ZTT

Z  - the zoom level (0-4).
TT - the texture id.
```
Because Maxis set the upper 20 bits of their terrain texture ids to 0, we use those 20 bits as an offset
to separate terrain textures that use the same ZTT values.
The Texture Index id is used for the upper 16 bits of the 20 bit offset code.

```
0xABCDEZTT

ABCD - the user's Texture Index id.
E    - reserved.
Z    - the zoom level (0-4).
TT   - the texture id.
```

#### Conflicting Texture Offsets

To prevent conflicts with existing texture instance ids in the 0x7AB50E44, 0x891B0E1A group, some texture offsets
are reserved for use by the game.

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

#### Example INI Section

The key is the terrain mod prefix name, and the value is the offset for that terrain mod.

```ini
[TextureOffsets]
Desert=0x12345
```

## ExtendedTerrain INI

_SC4ExtendedTerrain.ini_ contains two options for logging the game's terrain texture loading.

The _LogTextureIDs_ option will log the texture ids as the game requests them.
The _LogTextureLoadErrors_ option will only log the texture ids that the game fails to load.

## Download

The plugin can be downloaded from the Releases tab: https://github.com/0xC0000054/sc4-extended-terrain/releases

## System Requirements

* SimCity 4 version 641
* Windows 10 or later

The plugin may work on Windows 7 or later with the [Microsoft Visual C++ 2022 x86 Redistribute](https://aka.ms/vs/17/release/vc_redist.x86.exe) installed, but I do not have the ability to test that.

## Installation

1. Close SimCity 4.
2. Copy _SC4ExtendedTerrain.dll_, _SC4ExtendedTerrain.ini_ and _ExtendedTerrainUI.dat_ into the top-level of the Plugins folder in the SimCity 4 installation directory or Documents/SimCity 4 directory.
3. Start SimCity 4.

## Usage

The DLL adds a _ExtendedTerrain_ cheat when in the region view. This cheat code has no parameters and shows a dialog that
allows the terrain type to be changed for that region.

## Troubleshooting

The plugin should write a `SC4ExtendedTerrain.log` file in the same folder as the plugin.    
The log contains status information for the most recent run of the plugin.

# License

This project is licensed under the terms of the MIT License.    
See [LICENSE.txt](LICENSE.txt) for more information.

## 3rd party code

[gzcom-dll](https://github.com/nsgomez/gzcom-dll/tree/master) Located in the vendor folder, MIT License.    
[EABase](https://github.com/electronicarts/EABase) Located in the vendor folder, BSD 3-Clause License.    
[EASTL](https://github.com/electronicarts/EASTL) Located in the vendor folder, BSD 3-Clause License.    
[Windows Implementation Library](https://github.com/microsoft/wil) - MIT License    
[SC4Fix](https://github.com/nsgomez/sc4fix) - MIT License.    
[Frozen](https://github.com/serge-sans-paille/frozen) - Apache 2.0 License.    
[Boost.Algorithm](https://www.boost.org/doc/libs/1_84_0/libs/algorithm/doc/html/index.html) - Boost Software License, Version 1.0.    
[Boost.Endian](https://www.boost.org/doc/libs/1_84_0/libs/endian/doc/html/endian.html) - Boost Software License, Version 1.0.    
[Boost.PropertyTree](https://www.boost.org/doc/libs/1_84_0/doc/html/property_tree.html) - Boost Software License, Version 1.0.

# Source Code

## Prerequisites

* Visual Studio 2022

## Building the plugin

* Open the solution in the `src` folder
* Update the post build events to copy the build output to you SimCity 4 application plugins folder.
* Build the solution

## Debugging the plugin

Visual Studio can be configured to launch SimCity 4 on the Debugging page of the project properties.
I configured the debugger to launch the game in a window with the following command line:    
`-intro:off -CPUcount:1 -w -CustomResolution:enabled -r1920x1080x32`

You may need to adjust the resolution for your screen.
