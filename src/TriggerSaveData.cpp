#include <SDL3/SDL_log.h>

#include "TriggerSaveData.h"

using json = nlohmann::json;

json TriggerSaveData::ToJSON() const
{
    json j;

    j["id"] = id;
    j["name"] = name;
    j["runtime"] = isRuntimeFlag;
    j["exists"] = existsFlag;

    if (!runtimeState.is_null() && !runtimeState.empty())
    {
        j["state"] = runtimeState;
    }

    if (isRuntimeFlag && !templateData.is_null() && !templateData.empty())
    {
        j["template"] = templateData;
    }

    return j;
}

TriggerSaveData TriggerSaveData::FromJSON(const nlohmann::json& triggerJSON)
{
    /*
        What this function expects:

        {
            "id": 1837461827364123,
            "name": "triggerName"
            "runtime": false,
            "exists": true,
            "state": {
                "triggered": true,
                "timesFired": 2,
                "lastActivation": 132
            }
        }

        {
            "id": 981273981273,
            "name": "triggerName2"
            "runtime": true,
            "exists": true,
            "template": { <triggerTemplate> },
            "state": {
                "triggered": false,
                "timesFired": 0,
                "lastActivation": 0
            }
        }
    */

    TriggerSaveData data{};

    if (!triggerJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TriggerSaveData.FromJSON: Expected object, got non-object.");
        return data;
    }

    if (!triggerJSON.contains("id") || !triggerJSON["id"].is_number_unsigned())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TriggerSaveData.FromJSON: Missing or invalid 'id'.");
        return data;
    }

    data.id = triggerJSON.at("id").get<uint64_t>();

    if (!triggerJSON.contains("runtime") || !triggerJSON["runtime"].is_boolean())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TriggerSaveData.FromJSON: Missing or invalid 'runtime' flag. Defaulting to false.");
    }

    if (!triggerJSON.contains("exists") || !triggerJSON["exists"].is_boolean())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TriggerSaveData.FromJSON: Missing or invalid 'exists' flag. Defaulting to true.");
    }

    data.isRuntimeFlag = triggerJSON.value("runtime", false);
    data.existsFlag = triggerJSON.value("exists", true);

    if (!triggerJSON.contains("state") || !triggerJSON["state"].is_object())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TriggerSaveData.FromJSON: Missing or invalid 'state' object. Defaulting to default values.");
    }

    data.runtimeState = triggerJSON.value("state", json::object());

    if (data.isRuntimeFlag &&
        triggerJSON.contains("template") &&
        triggerJSON["template"].is_object())
    {
        data.templateData = triggerJSON["template"];
    }
    else
    {
        data.templateData = json::object();
    }

    return data;
}