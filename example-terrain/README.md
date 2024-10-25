# Example Terrain

This folder contains the following example files:

## EditedTerrain.ini

This file is an edited version of SC4's default _terrain.ini_ entry that adds a Desert terrain mode in addition to the standard
Tropical terrain mode.
The desert terrain mode consists of a _DesertMiscTextures_ section that is the same as the _TropicalMiscTextures_ section, and a
_DesertTextureMapTable_ with all values set to the Tropical Beach texture.

## EditedTerrainIni.dat

This file contains _EditedTerrain.ini_ imported as an override for the game's default terrain.ini. The game's terrain.ini entry
uses a TGI value of 0x00000000, 0x8a5971c5, 0xaa597172.