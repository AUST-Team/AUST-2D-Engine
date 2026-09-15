#include <SDL3/SDL_log.h>

#include "GuardSaveData.h"
#include "JSONParser.h"

using json = nlohmann::json;

json GuardSaveData::ToJSON() const
{
    return {
        {"id", guardId },
        { "position", { { "x", guardPosition.x }, { "y", guardPosition.y } } }
    };
}

GuardSaveData GuardSaveData::FromJSON(const json& guardJSON)
{
    /*
        What this function expects:

        Note that 'guardJSON' is the default JSON object (though usually it's a part of a vector of "guards": [guard1, guard2, ...])

        {
            "id": 0,
            "position": { "x": 144, "y": 144 }
        },
    */

    GuardSaveData g{};

    if (!guardJSON.is_object())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GuardSaveData.FromJSON: Expected object, got non-object.");
        return g;
    }

    if (!guardJSON.contains("id") || !guardJSON["id"].is_number_unsigned())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GuardSaveData.FromJSON: Missing or invalid 'id' object. Defaulting to -1.");
    }

    g.guardId = guardJSON.value("id", -1);

    if (guardJSON.contains("position") && (guardJSON["position"].is_object() || guardJSON["position"].is_array() || guardJSON["position"].is_number()))
    {
        g.guardPosition = JSONParser::ParseJSONPoint<SDL_Point>(guardJSON["position"], SDL_Point{ 0, 0 });
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GuardSaveData.FromJSON: Missing or invalid 'position' object / array / number. Defaulting to {0, 0}.");
    }

    return g;
}