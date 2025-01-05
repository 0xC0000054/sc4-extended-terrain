////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-extended-terrain, a DLL Plugin for SimCity 4
// that adds support for more terrain types.
//
// Copyright (c) 2024, 2025 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "cIGZUnknown.h"

static const uint32_t GZIID_ITerrainS3DTextureBindingFactoryProxy = 0x96E97237;

class ITerrainS3DTextureBindingFactoryProxy : public cIGZUnknown
{
public:
	virtual bool IsInitialized() const = 0;
};