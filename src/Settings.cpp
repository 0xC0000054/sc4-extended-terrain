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
