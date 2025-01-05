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
