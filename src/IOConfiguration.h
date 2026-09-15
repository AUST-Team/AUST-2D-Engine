#pragma once

#ifndef IOCONFIGURATION_H_
#define IOCONFIGURATION_H_

#include <filesystem>
#include <string>

#include "ConstantConfiguration.h"

namespace fs = std::filesystem;

/**
* @brief Configuration for IO, such as file paths.
*/
struct IOConfiguration
{
    /**
    * @brief Returns the path to the configuration file. The path is initialized on the first call and can be modified.
    * 
    * @return The path to the configuration file.
    */
    static fs::path& GetConfigFilePath()
    {
        static fs::path path = ConstantConfiguration::defaultGameDirectoryPath / "configuration.json";
        return path;
    }

    std::string startingMap = "DebugMap";           /// Starting map name.
    fs::path mapDirectory = fs::path("resources") / "maps";       /// Path to the maps folder.

    fs::path uiFilePath = fs::path("resources") / "ui.json";      /// Path to the UI JSON file.
    fs::path uiDirectory = fs::path("resources") / "ui";          /// Path to the UI directory.

    fs::path saveDirectory = "saves";               /// Saves directory.

    fs::path scoreFilePath = fs::path("scores") / "scores.json";  /// Path to the scores file.

    fs::path fontFilePath = fs::path("resources") / "fonts.json";     /// Path to the font JSON file.
    fs::path fontDirectoryPath = fs::path("resources") / "fonts";     /// Path to the font directory.

    fs::path spriteSheetFilePath = fs::path("resources") / "textures" / "sprite_sheet.png"; /// Path to the sprite sheet.
    fs::path tileFilePath = fs::path("resources") / "tiles.json";     /// Path to the tiles file.

    fs::path presetDirectory = fs::path("resources") / "presets";     /// Path to the preset folder.
    fs::path flagFilePath = fs::path("resources") / "flags.json";     /// Path to the flags file.

    fs::path imagesDirectory = fs::path("resources") / "images";      /// Path to the images directory.

    fs::path audioFilePath = fs::path("resources") / "audio.json";    /// Path to the audio file.
    fs::path audioDirectory = fs::path("resources") / "audio";        /// Path to the audio folder.

    fs::path settingsFilePath = fs::path("resources") / "settings.json";    /// Path to the settings file.

    fs::path cutsceneDirectory = fs::path("resources") / "cutscenes";   /// Path to the cutscene directory.

    fs::path dialogueDirectory = fs::path("resources") / "dialogue";    /// Path to the dialogue directory.

    fs::path systemValuesFilePath = fs::path("resources") / "systemValues.json";    /// Path to the system values JSON.
};

#endif // IOCONFIGURATION_H_