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
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/ini_parser.hpp"
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
		std::ifstream stream(FileSystem::GetConfigFilePath(), std::ifstream::in);

		if (stream)
		{
			boost::property_tree::ptree tree;
			boost::property_tree::ini_parser::read_ini(stream, tree);

			logTextureIDs = tree.get<bool>("ExtendedTerrain.LogTextureIDs");
			logTextureLoadErrors = tree.get<bool>("ExtendedTerrain.LogTextureLoadErrors");
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
