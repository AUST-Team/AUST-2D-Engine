#include <iostream>
#include <cstdlib>
#include <SDL3/SDL_log.h>
#include <fstream>
#include <nlohmann/json.hpp>

#include "ModManager.h"

#include "IOConfiguration.h"
#include "FileLoader.h"
#include "IOMiscs.h"

namespace fs = std::filesystem;
using json = nlohmann::json;

void ModManager::ScanForMods(const fs::path& modsDirectory)
{
    availableMods.clear();

    fs::path baseDir = ConstantConfiguration::defaultGameDirectoryPath;

    if(!fs::exists(baseDir) || !fs::is_directory(baseDir))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ModManager.ScanForMods: Base game directory [%s] not found. Are you sure you know what you're doing?.", ConstantConfiguration::defaultGameDirectoryPath.string().c_str());
	}

	std::optional<ModMetadata> baseGameMod = ReadModMetadata(fs::path(ConstantConfiguration::defaultGameDirectoryPath));

    if (baseGameMod)
    {
        availableMods.push_back(*baseGameMod);
    }
    else
    {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ModManager.ScanForMods: Failed to read base game mod metadata from [%s].", ConstantConfiguration::defaultGameDirectoryPath.string().c_str());
    }

    if (!fs::exists(modsDirectory) || !fs::is_directory(modsDirectory))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ModManager.ScanForMods: Mod directory [%s] not found.", modsDirectory.string().c_str());
        return;
    }

    for (const fs::directory_entry& entry : fs::directory_iterator(modsDirectory))
    {
        if (!entry.is_directory())
        {
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ModManager.ScanForMods: Found non-directory entry [%s] inside mods folder. Skipping.", entry.path().filename().string().c_str());
            continue;
        }

        std::string folderName = entry.path().filename().string();

        if (folderName == ConstantConfiguration::defaultGameDirectoryName)
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ModManager.ScanForMods: Found folder matching base game directory name [%s] inside mods folder. Skipping.", folderName.c_str());
            continue;
        }

        if (ignoredFolders.count(folderName))
        {
            continue;
        }

        std::optional<ModMetadata> modMetadata = ReadModMetadata(entry.path());

        if (modMetadata)
        {
            availableMods.push_back(*modMetadata);
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ModManager.ScanForMods: Failed to read mod metadata for folder [%s]. Skipping.", folderName.c_str());
        }
    }
}

void ModManager::SetActiveMod(const std::string& modFolderName)
{
    if (modFolderName.empty() || modFolderName == ConstantConfiguration::defaultGameDirectoryName)
    {
        activeModPath = ConstantConfiguration::defaultGameDirectoryPath;
    }
    else
    {
        activeModPath = ConstantConfiguration::defaultModsDirectory / modFolderName;
    }
}

void ModManager::SetActiveModConfiguration(const std::string& modFolderName)
{
    SetActiveMod(modFolderName);
    auto it = std::find_if(availableMods.begin(), availableMods.end(), [&modFolderName](const ModMetadata& mod) {
        return mod.folderName == modFolderName;
    });

    if (it != availableMods.end())
    {
        IOConfiguration::GetConfigFilePath() = it->configPath;
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ModManager.SetActiveModConfiguration: Mod [%s] metadata not found. Falling back to default configuration.", modFolderName.c_str());
        IOConfiguration::GetConfigFilePath() = ResolvePath(fs::path("configuration.json"));
    }
}

fs::path ModManager::ResolvePath(const fs::path& relativePath) const
{
    fs::path crossRelativePath = IOMiscs::MakeCrossPlatformPath(relativePath);
    fs::path baseDirectory = ConstantConfiguration::defaultGameDirectoryPath;

    if (activeModPath == baseDirectory)
    {
        return (baseDirectory / crossRelativePath).make_preferred();
    }

    fs::path modSpecificPath = activeModPath / crossRelativePath;
    if (fs::exists(modSpecificPath))
    {
        return modSpecificPath.make_preferred();
    }

    return (baseDirectory / crossRelativePath).make_preferred();
}

bool ModManager::HasMod(const std::string& modFolderName) const
{
    fs::path targetPath;
    if (modFolderName == ConstantConfiguration::defaultGameDirectoryName)
    {
        targetPath = ConstantConfiguration::defaultGameDirectoryPath;
    }
    else
    {
        targetPath = ConstantConfiguration::defaultModsDirectory / modFolderName;
    }

    // Check if the directory and manifest exist manually.
    return fs::exists(targetPath) && fs::exists(targetPath / "mod.json");
}

std::optional<ModMetadata> ModManager::ReadModMetadata(const fs::path& modFolderPath) const
{
    if (!fs::exists(modFolderPath) || !fs::is_directory(modFolderPath))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ModManager.ReadModMetadata: Mod directory [%s] not found.", modFolderPath.filename().string().c_str());
        return std::nullopt;
    }

    fs::path manifestPath = modFolderPath / "mod.json";
    std::string folderName = modFolderPath.filename().string();

    if (fs::exists(manifestPath))
    {
        try
        {
            std::optional<json> jsonOptional = FileLoader::LoadJSON(manifestPath, JSONParseFlags::None);

            if (!jsonOptional)
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ModManager.ReadModMetadata: Could not open mod file \'%s\'", manifestPath.string().c_str());
                return std::nullopt;
            }

            const json& j = *jsonOptional;
            ModMetadata mod;

            mod.folderName = folderName;
            mod.modName = j.value("name", folderName);

            std::string localConfig = j.value("configFile", "configuration.json");
            mod.configPath = IOMiscs::MakeCrossPlatformPath(modFolderPath / localConfig);

            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "ModManager.ReadModMetadata: Discovered Mod: %s (%s)", mod.modName.c_str(), folderName.c_str());
            return mod;
        }
        catch (const std::exception& e)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ModManager.ReadModMetadata: Failed to parse mod manifest for [%s]: %s", folderName.c_str(), e.what());
            return std::nullopt;
        }
    }
    else
    {
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ModManager.ReadModMetadata: No manifest found for mod folder [%s]. Using default metadata values.", folderName.c_str());
        return std::nullopt;
    }
}

