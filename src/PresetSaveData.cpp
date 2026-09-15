#include <SDL3/SDL_log.h>

#include "PresetSaveData.h"
#include "StringMiscs.h"
#include "JSONParser.h"

using json = nlohmann::json;

json PresetSaveData::ToJSON() const
{
	json j;

    j["position"] = { {"x", position.x}, {"y", position.y} };
	j["preset"] = name;

    if (!IsSentinel(mode))
    {
        std::string&& placement = TilePlacementModeToString(mode);

        if (StringMiscs::IsStringEnumSentinel(placement))
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "PresetSaveData.ToJSON: Unknown tile placement mode. Defaulting to 'Replace'");
            placement = TilePlacementModeToString(TilePlacementMode::Replace);
        }

        j["mode"] = placement;
    }

	return j;
}

PresetSaveData PresetSaveData::FromJSON(const json& presetJSON)
{
    PresetSaveData data;

    /*
        What this function expects (example):

        Note that 'presetJSON' is the default JSON object, but usually it's part of an array [preset1, preset2, ...].

        {
            "position": { "x": 10, "y": 5 },
            "preset": "Lake",
            "mode": "Replace"
        }

        {
            "position": { "x": 1, "y": 2 },
            "preset": "House1"
        }
    */

    if (!presetJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "PresetSaveData.FromJSON: Expected object, got non-object.");
        return data;
    }

    if (!presetJSON.contains("preset") || !presetJSON["preset"].is_string())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "PresetSaveData.FromJSON: 'preset' [name] is missing or not string. Skipping.");
        return data;
    }

    data.name = presetJSON.value("preset", "");

    if (presetJSON.contains("position") && (presetJSON["position"].is_object() || presetJSON["position"].is_array() || presetJSON["position"].is_number()))
    {
        data.position = JSONParser::ParseJSONPoint<SDL_Point>(presetJSON["position"], SDL_Point { 0, 0 });
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "PresetSaveData.FromJSON: 'position' is missing or not object / array / number or for presetJSON [%s]. Defaulting to {0, 0}.", data.name.c_str());
    }

    data.mode = GetSentinel<TilePlacementMode>();

    if (presetJSON.contains("mode") && presetJSON["mode"].is_string())
    {
        const std::string& modeStr = presetJSON.at("mode").get_ref<const std::string&>();
        data.mode = TilePlacementModeFromString(modeStr);

        if(IsSentinel(data.mode))
        { 
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "PresetSaveData.FromJSON: Placement mode returned nullopt for string [%s] for presetJSON [%s]. Defaulting to tile defaults.", modeStr.c_str(), data.name.c_str());
            data.mode = TilePlacementMode::UseTileDefault;
        }
    }

    return data;
}