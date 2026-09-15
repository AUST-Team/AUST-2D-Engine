#pragma once

#ifndef TILESAVADATA_H_
#define TILESAVEDATA_H_

#include <SDL3/SDL_rect.h>
#include <nlohmann/json.hpp>

#include "TilePlacementMode.h"

/**
* @brief Structure for a tile save data.
*/
struct TileSaveData
{
    SDL_Point position = { 0, 0 };
    int tileIndex = -1; /// Index of the tile.
    TilePlacementMode mode = TilePlacementMode::Replace; /// Placement mode of the tile.

    /**
    * @brief Converts the tile save data to JSON format.
    *
    * @return a JSON containing the tile save data.
    */
    nlohmann::json ToJSON() const;

    /**
    * @brief Converts a JSON format to tile save data.
    *
    * @param tileJSON JSON containing the tile data in JSON format.
    *
    * @return A TileSaveData structure containing the tile data.
    */
    static TileSaveData FromJSON(const nlohmann::json& tileJSON);
};

#endif // TILESAVADATA_H_