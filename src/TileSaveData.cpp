#include <SDL3/SDL_log.h>

#include "TileSaveData.h"
#include "TileRegistry.h"
#include "StringMiscs.h"
#include "JSONParser.h"

using json = nlohmann::json;

json TileSaveData::ToJSON() const
{
	json j;

    j["position"] = { {"x", position.x}, {"y", position.y} };
	j["tile"] = TileRegistry::GetTileNameByIndex(tileIndex);
	std::string&& placement = TilePlacementModeToString(mode);

    if (!StringMiscs::IsStringEnumSentinel(placement))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileSaveData.ToJSON: Unknown tile placement mode. Defaulting to 'Replace'.");
        placement = TilePlacementModeToString(TilePlacementMode::Replace);
    }

    j["mode"] = placement;

	return j;
}

TileSaveData TileSaveData::FromJSON(const json& tileJSON)
{
    TileSaveData data{};

    /*
        What this function expects (example):

        Note that 'tileJSON' is the default JSON object, but usually it's part of an array [tile1, tile2, ...].

        { 
            "position": { "x": 10, "y": 10 }, 
            "tileJSON": "TownSoil", 
            "mode": "Replace" 
        }
    */

    if (!tileJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TileSaveData.FromJSON: Expected object, got non-object.");
        return data;
    }

    if (tileJSON.contains("position") && (tileJSON["position"].is_object() || tileJSON["position"].is_array() || tileJSON["position"].is_number()))
    {
        data.position = JSONParser::ParseJSONPoint<SDL_Point>(tileJSON["position"], SDL_Point{ 0, 0 });
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileSaveData.FromJSON: 'position' is missing or not object / array / number or for tileJSON. Defaulting to {0, 0}.");
    }

    if (!tileJSON.contains("tile") || !tileJSON["tile"].is_string())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileSaveData.FromJSON: 'tile' [name] is missing or not string. Defaulting to default invalid tile index.");
    }

    if (!tileJSON.contains("mode") || !tileJSON["mode"].is_string())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileSaveData.FromJSON: 'mode' [placement mode] is missing or not string. Defaulting to 'Replace'.");
    }

    const std::string& tileName = tileJSON.at("tile").get_ref<const std::string&>();
    data.tileIndex = TileRegistry::GetTileIndexByName(tileName);

    const std::string& modeStr = tileJSON.at("mode").get_ref<const std::string&>();
    TilePlacementMode mode = TilePlacementModeFromString(modeStr);
    data.mode = IsSentinel(mode) ? TilePlacementMode::Replace : mode;

    return data;
}