#pragma once

#ifndef GAMESAVEDATA_H_
#define GAMESAVEDATA_H_

#include <nlohmann/json.hpp>
#include <vector>
#include <SDL3/SDL_rect.h>

#include "TriggerSaveData.h"
#include "GuardSaveData.h"
#include "TileSaveData.h"
#include "PresetSaveData.h"

/**
* @brief Structure containing the save data of the game.
*/
struct GameSaveData
{
    std::string flags;              /// String of hexadecimals containing the game flags.
    std::string timestamp;          /// Save data's timestamp.
    std::string playerDirection;    /// The player's direction.
    std::string mapName;            /// ID of the map.
    std::vector<TriggerSaveData> triggers;  /// Vector containing the trigger data of the map.
    std::vector<GuardSaveData> guards;  /// Vector containing the guard data of the map.
    std::vector<TileSaveData> tiles;    /// Vector containing the tile data of the map.
    std::vector<PresetSaveData> presets;    /// Vector containing the preset data of the map.
    uint64_t timePlayedSeconds = 0;     /// Total time played in seconds.
    int playerTimesCaught = -1;     /// Number of times the player has been caught.
    int id = -1;    /// ID of the save data.
    SDL_Point playerPosition = { 0, 0 };    /// Positon of the player.

    /**
    * @brief Converts the save data to JSON format.
    *
    * @return A JSON containing the save data.
    */
    nlohmann::json ToJSON() const;

    /**
    * @brief Converts a JSON format to save data.
    *
    * @param saveDataJSON JSON containing the save data.
    * 
    * @return A GameSaveData structure containing the save data.
    */
    static GameSaveData FromJSON(const nlohmann::json& saveDataJSON);
};

#endif // GAMESAVEDATA_H_
