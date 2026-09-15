#pragma once

#ifndef MODMANAGER_H_
#define MODMANAGER_H_

#include <vector>
#include <string>
#include <unordered_set>
#include <optional>
#include <filesystem>

#include "ModMetadata.h"
#include "ConstantConfiguration.h"

/**
* @brief Manager for mods. Responsible for scanning mod directories, storing discovered mods, and resolving file paths based on active mods.
*/
class ModManager
{
private:
	std::vector<ModMetadata> availableMods; /// List of discovered mods with their metadata.
	std::filesystem::path activeModPath = ConstantConfiguration::defaultGameDirectoryPath;    /// Path to the currently active mod's folder.

    const std::unordered_set<std::string> ignoredFolders = {
		".git", ".vs", "Debug", "x64", "Release", ".vscode", "MemoryLogs", "VLDMLogs"
    };  /// Set of folder names to ignore.

    // Private constructor / destructor.
	ModManager() = default;
	~ModManager() = default;

	// No copying or moving.
	ModManager(const ModManager&) = delete;
	ModManager& operator=(const ModManager&) = delete;
	ModManager(ModManager&&) = delete;
	ModManager& operator=(ModManager&&) = delete;

    /**
	* @brief Reads the mod metadata from a given mod folder path. Expects a 'mod.json' file in the folder with the mod's metadata.
    * 
	* @param modFolderPath The path to the mod folder to read the metadata from.
    * 
	* @return The ModMetadata read from the mod folder. If the 'mod.json' file is missing or invalid, returns an std::nullopt.
    */
    std::optional<ModMetadata> ReadModMetadata(const std::filesystem::path& modFolderPath) const;

    std::optional<ModMetadata> ReadModMetadata(const std::string& modFolderPath) const = delete;
    std::optional<ModMetadata> ReadModMetadata(const char* modFolderPath) const = delete;

public:

    /**
    * @brief Returns the singleton instance of the ModManager.
    * 
	* @return Reference to the singleton instance of the ModManager.
    */
    static ModManager& GetInstance()
    {
        static ModManager instance;
        return instance;
	}

    /**
	* @brief Scans the specified mods directory for available mods. Each mod should be in its own folder with a 'mod.json' manifest file.
    * 
	* @param modsDirectory The path to the directory containing mod folders. Each mod folder should contain a 'mod.json' file with the mod's metadata.
    */
    void ScanForMods(const std::filesystem::path& modsDirectory = ConstantConfiguration::defaultModsDirectory);

    /**
	* @brief Sets the active mod by its folder name. The active mod's assets will take precedence over the base game assets when resolving file paths.
    * 
	* This just sets the active mod, it does not change the active configuration path. Use 'SetActiveModConfiguration' for that, or set the configuration path with ResolvePath("configuration.json").
    * 
	* @param modFolderName The folder name of the mod to set as active. Should match the 'folderName' field in the mod's metadata.
    */
    void SetActiveMod(const std::string& modFolderName = ConstantConfiguration::defaultGameDirectoryName);

    /**
	* @brief Sets the active configuration path to a given mod's.
    * 
	* @param modFolderName The folder name of the mod to set the active configuration path to. Should match the 'folderName' field in the mod's metadata.
    */
	void SetActiveModConfiguration(const std::string& modFolderName = ConstantConfiguration::defaultGameDirectoryName);

    /**
	* @brief Resolves a file path based on the active mod. If the file exists in the active mod's folder, that path is returned. Otherwise, the path to the base game asset is returned.
    * 
	* @param relativePath The relative path to the asset, relative to the mod folder or base game folder.
    * 
    * @return The relative full path from the game executable to the given asset.
    */
    std::filesystem::path ResolvePath(const std::filesystem::path& relativePath) const;

    /**
	* @brief Checks if a mod with the given folder name exists in the available mods.
    * 
	* @param modFolderName The folder name of the mod to check for.
    * 
	* @return 'true' if a mod with the given folder name exists, 'false' if otherwise.
    */
    bool HasMod(const std::string& modFolderName) const;

    void ScanForMods(const std::string& modsDirectory) = delete;
    void ScanForMods(const char* modsDirectory) = delete;
    std::filesystem::path ResolvePath(const std::string& relativePath) const = delete;
    std::filesystem::path ResolvePath(const char* relativePath) const = delete;
};

#endif // MODMANAGER_H_