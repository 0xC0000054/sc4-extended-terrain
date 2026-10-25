/*
 * This file is part of sc4-extended-terrain, a DLL Plugin for SimCity 4
 * that adds support for more terrain types.
 *
 * Copyright (C) 2024, 2025 Nicholas Hayes
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

#include "Settings.h"
#include "FileSystem.h"
#include "Logger.h"
#include "IniReader.h"
#include <fstream>

Settings::Settings()
	: logTextureIDs(true),
	  logTextureLoadErrors(false)
{
}

bool Settings::LogLoadedTextureIDs() const
{
	return logTextureIDs;
}

bool Settings::LogTextureLoadErrors() const
{
	return logTextureLoadErrors;
}

void Settings::Load()
{
	Logger& logger = Logger::GetInstance();

	try
	{
		std::ifstream stream(FileSystem::GetDllIniFilePath(), std::ifstream::in);

		if (stream)
		{
			IniReader iniReader(stream);

			const IniSection& extendedTerrainSection = iniReader.get_section("ExtendedTerrain");

			logTextureIDs = extendedTerrainSection.get_converted_value<bool>("LogTextureIDs");
			logTextureLoadErrors = extendedTerrainSection.get_converted_value<bool>("LogTextureLoadErrors");
		}
		else
		{
			logger.WriteLine(LogLevel::Error, "Failed to open the settings file.");
		}
	}
	catch (const std::exception& e)
	{
		logger.WriteLineFormatted(
			LogLevel::Error,
			"Error reading the settings file: %s",
			e.what());
	}
}
