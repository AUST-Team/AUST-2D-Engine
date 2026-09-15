#include <sstream>
#include <nlohmann/json.hpp>
#include <SDL3/SDL_log.h>

#include "GameSaveData.h"
#include "JSONParser.h"

using json = nlohmann::json;

json GameSaveData::ToJSON() const
{
    json j;
    j["id"] = id;
    j["mapName"] = mapName;
    j["player"] = {
        {"playerPosition", { { "x", playerPosition.x }, { "y", playerPosition.y } } },
        {"playerDirection", playerDirection}
    };
    j["flags"] = flags;
    j["timestamp"] = timestamp;
    j["timePlayedSeconds"] = timePlayedSeconds;
    j["timesCaught"] = playerTimesCaught;

    json guardArray = json::array();
    for (const GuardSaveData& guard : guards)
    {
        guardArray.push_back(guard.ToJSON());
    }
    j["guards"] = guardArray;

    json triggerArray = json::array();
    for (const TriggerSaveData& trigger : triggers)
    {
        triggerArray.push_back(trigger.ToJSON());
    }
    j["triggers"] = triggerArray;

    json tileArray = json::array();
    for (const TileSaveData& tile : tiles)
    {
        tileArray.push_back(tile.ToJSON());
    }
    j["tiles"] = tileArray;

    json presetArray = json::array();
    for (const PresetSaveData& preset : presets)
    {
        presetArray.push_back(preset.ToJSON());
    }
    j["presets"] = presetArray;

    return j;
}

// Top 10 loggers.
// Coming back to this comment (made in 2025!) is really bitter-sweet. This function is tiny in terms of logging (see UIFactory),
// but at the time, it was one of the biggest.
GameSaveData GameSaveData::FromJSON(const json& saveDataJSON)
{
    GameSaveData save;

    /*
        What this function expects (example): 

        Note that saveDataJSON is the default JSON object.

        {
            "flags": "2,",
            "guards": [
                {
                    "position": { "x": 144, "y": 144 }
                },
                {
                    "position": { "x": 240, "y": 240 }
                }
            ],

            "id": 1,
            "mapId": 0,
            "playerPosition": { "x": 528, "y": 96 },
            "timePlayedSeconds": 28,
            "timesCaught": 0,
            "timestamp": "2025-09-12 22:33:26"
        }
    */

    if (!saveDataJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameSaveData.FromJSON: Expected object, got non-object.");
        return save;
    }

    if (!saveDataJSON.contains("id") || !saveDataJSON["id"].is_number_unsigned())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameSaveData.FromJSON: Missing or invalid 'id'. Defaulting to 1.");
    }

    if (!saveDataJSON.contains("mapName") || !saveDataJSON["mapName"].is_string())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameSaveData.FromJSON: Missing or invalid 'mapName'. Defaulting to 'DebugMap'.");
    }

    if (!saveDataJSON.contains("player") || !saveDataJSON["player"].is_object())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameSaveData.FromJSON: Missing or invalid 'player'. Defaulting to default values.");
    }

    const json player = saveDataJSON.value("player", json::object());

    if (player.contains("playerPosition") && (player["playerPosition"].is_object() || player["playerPosition"].is_array() || player["playerPosition"].is_number()))
    {
        save.playerPosition = JSONParser::ParseJSONPoint<SDL_Point>(player["playerPosition"], SDL_Point { 0, 0 });
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameSaveData.FromJSON: 'playerPosition' is missing or not object / array / number. Defaulting to {0, 0}.");
    }

    if (!player.contains("playerDirection") || !player["playerDirection"].is_string())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameSaveData.FromJSON: Missing or invalid 'playerDirection'. Defaulting to Right.");
    }

    if (!saveDataJSON.contains("flags") || !saveDataJSON["flags"].is_string())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameSaveData.FromJSON: Missing or invalid 'flags'. Defaulting to empty string.");
    }

    if (!saveDataJSON.contains("timestamp") || !saveDataJSON["timestamp"].is_string())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameSaveData.FromJSON: Missing or invalid 'timestamp'. Defaulting to empty string.");
    }

    if (!saveDataJSON.contains("timePlayedSeconds") || !saveDataJSON["timePlayedSeconds"].is_number_integer())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameSaveData.FromJSON: Missing or invalid 'timePlayedSeconds'. Defaulting to 0.");
    }

    if (!saveDataJSON.contains("timesCaught") || !saveDataJSON["timesCaught"].is_number_unsigned())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameSaveData.FromJSON: Missing or invalid 'timesCaught'. Defaulting to 0.");
    }

    save.id = saveDataJSON.value("id", 1);
    save.mapName = saveDataJSON.value("mapName", "DebugMap");

    save.playerDirection = player.value("playerDirection", "Right");
    save.flags = saveDataJSON.value("flags", "");
    save.timestamp = saveDataJSON.value("timestamp", "");
    save.timePlayedSeconds = saveDataJSON.value("timePlayedSeconds", 0L);
    save.playerTimesCaught = saveDataJSON.value("timesCaught", 0u);

    if (!saveDataJSON.contains("guards") || !saveDataJSON["guards"].is_array())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameSaveData.FromJSON: Missing or invalid 'guards' array. No guards will be loaded.");
    }
    else
    {
        for (const json& g : saveDataJSON["guards"])
        {
            if (!g.is_object())
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameSaveData.FromJSON: Guard entry is not object.");
                continue;
            }

            save.guards.push_back(GuardSaveData::FromJSON(g));
        }
    }

    if (!saveDataJSON.contains("triggers") || !saveDataJSON["triggers"].is_array())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameSaveData.FromJSON: Missing or invalid 'triggers' array. No triggers will be loaded.");
    }
    else
    {
        for (const json& t : saveDataJSON["triggers"])
        {
            if (!t.is_object())
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameSaveData.FromJSON: Trigger save entry is not object.");
                continue;
            }

            save.triggers.push_back(TriggerSaveData::FromJSON(t));
        }
    }

    if (!saveDataJSON.contains("tiles") || !saveDataJSON["tiles"].is_array())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameSaveData.FromJSON: Missing or invalid 'tiles' array. No tile changes will be loaded.");
    }
    else
    {
        for (const json& t : saveDataJSON["tiles"])
        {
            if (!t.is_object())
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameSaveData.FromJSON: Tile change entry is not object.");
                continue;
            }

            save.tiles.push_back(TileSaveData::FromJSON(t));
        }
    }

    if (!saveDataJSON.contains("presets") || !saveDataJSON["presets"].is_array())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameSaveData.FromJSON: Missing or invalid 'tiles' array. No tile changes will be loaded.");
    }
    else
    {
        for (const json& p : saveDataJSON["presets"])
        {
            if (!p.is_object())
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameSaveData.FromJSON: Preset change entry is not object.");
                continue;
            }

            save.presets.push_back(PresetSaveData::FromJSON(p));
        }
    }

    return save;
}
