#pragma once

#ifndef GUARDSAVEDATA_H_
#define GUARDSAVEDATA_H_

#include <SDL3/SDL_rect.h>
#include <nlohmann/json.hpp>

/**
* @brief Save data of a guard.
*/
struct GuardSaveData
{
    int guardId = -1;   /// ID of the guard.
    SDL_Point guardPosition = { 0, 0 };  /// Position of the guard.

    /**
    * @brief Converts the guard data to JSON format.
    *
    * @return A JSON containing the guard data.
    */
    nlohmann::json ToJSON() const;

    /**
    * @brief Converts a JSON format to guard data.
    *
    * @param guardJSON JSON containing the guard data in JSON format.
    *
    * @return A GuardSaveData structure containing the guard data.
    */
    static GuardSaveData FromJSON(const nlohmann::json& guardJSON);
};


#endif // GUARDSAVEDATA_H_