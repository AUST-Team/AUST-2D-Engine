#pragma once

#ifndef MODMETADATA_H_
#define MODMETADATA_H_

#include <string>
#include <filesystem>

/**
* @brief Structure for mod metadata.
*/
struct ModMetadata
{
    std::string folderName;    /// Name of the mod's folder. Should be unique across mods.
	std::string modName;       /// Name of the mod. Does not have to be unique across mods.
	std::filesystem::path configPath;    /// Path to the mod's configuration JSON file, relative to the mod folder.
};

#endif // MODMETADATA_H_