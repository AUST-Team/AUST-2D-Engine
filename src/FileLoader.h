#pragma once

#ifndef FILELOADER_H_
#define FILELOADER_H_

#include <nlohmann/json.hpp>
#include <string>
#include <filesystem>

#include "JSONParseFlags.h"

namespace FileLoader
{
	/**
	* @brief Loads a JSON file and returns the parsed JSON object.
	*
	* @param filePath The path to the JSON file.
	* @param parseFlags Parse flags flags (prase definitions, delete definitions, etc).
	*
	* @return The parsed JSON object, or std::nullopt if the file could not be loaded or parsed.
	*/
	std::optional<nlohmann::json> LoadJSON(const std::filesystem::path& filePath, JSONParseFlags loadingFlags = JSONParseFlags::ParseDefines | JSONParseFlags::DeleteDefines);

	std::optional<nlohmann::json> LoadJSON(const std::string& filePath, JSONParseFlags parseFlags) = delete;
	std::optional<nlohmann::json> LoadJSON(const char* filePath, JSONParseFlags parseFlags) = delete;
}

#endif // FILELOADER_H_