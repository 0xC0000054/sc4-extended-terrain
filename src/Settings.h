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
#include "ISettings.h"

class Settings final : public ISettings
{
public:
	Settings();

	void Load();

private:
	// ISettings

	bool LogLoadedTextureIDs() const override;
	bool LogTextureLoadErrors() const override;

	// Private members

	bool logTextureIDs;
	bool logTextureLoadErrors;
};
