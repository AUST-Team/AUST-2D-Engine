#pragma once

#ifndef PRESETSAVEDATA_H_
#define PRESETSAVEDATA_H_

#include <string>
#include <nlohmann/json.hpp>
#include <SDL3/SDL_rect.h>

#include "TilePlacementMode.h"

/**
* @brief Structure for preset save data.
*/
struct PresetSaveData
{
	std::string name;	/// Name of the preset.
    SDL_Point position = { 0, 0 };   /// Position of the preset (top left corner).
	TilePlacementMode mode = GetSentinel<TilePlacementMode>();	/// Placement mode of the preset.

    /**
    * @brief Converts the preset data to JSON format.
    *
    * @return A JSON containing the preset data.
    */
    nlohmann::json ToJSON() const;

    /**
    * @brief Converts a JSON format to preset data.
    *
    * @param presetJSON JSON containing the preset data in JSON format.
    *
    * @return A PresetSaveData structure containing the preset data.
    */
    static PresetSaveData FromJSON(const nlohmann::json& presetJSON);
};

#endif // PRESETSAVEDATA_H_