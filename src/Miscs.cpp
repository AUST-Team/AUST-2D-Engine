#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <sstream>
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>

#include "Miscs.h"

#include "Configuration.h"
#include "GuardStrategy.h"
#include "Guard.h"
#include "UIButton.h"
#include "MemoryTracker.h"

using json = nlohmann::json;

std::function<Direction()> MakeDirectionProviderFrom(GuardStrategy* strategy, Guard& guard)
{
    return [strategy, &guard]() -> Direction
        {
            return strategy ? strategy->GetDirection(guard) : GetSentinel<Direction>();
        };
}

std::function<void()> CreateCallbackFromJSON(const json& callbackJSON)
{
    /*
        What this function expects:
        "callbackJSON": "null"
        "callbackJSON": "eventName"

        "callbackJSON": {
            "event": "eventName",
            "params": NULL
        }

        "callbackJSON": {
            "event": "eventName",
            "params": {
                <eventParameters>
            }
        }
    */

    // No references can be used in the lambda parameter capture.
    if (callbackJSON.is_string())
    {
        std::string event = callbackJSON.get<std::string>();

        if (event.empty())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "CreateCallbackFromJSON: Event name is empty. Defaulting to no-op.");
            return [](){};
        }

        return [event]() {
            //GameActionRegistry::GetInstance().Execute(event);
        };
    }
    else if (callbackJSON.is_object())
    {
        if (!callbackJSON.contains("event") || !callbackJSON["event"].is_string())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "CreateCallbackFromJSON: JSON does not contain event name or is not a string. Defaulting to no-op.");
            return [](){};
        }

        std::string event = callbackJSON.value("event", "");

        if (!callbackJSON.contains("params") && !callbackJSON["params"].is_object())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "CreateCallbackFromJSON: JSON does not contain event parameters or is not an object. Defaulting to empty object.");
        }

        json params = callbackJSON.value("params", json::object());
        return [event, params]() {
            //GameActionRegistry::GetInstance().Execute(event, params);
        };
    }
    else if (callbackJSON.is_null())
    {
        // Returning no-op on purpose if there is no callbackJSON to be made.
        return [](){};
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "CreateCallbackFromJSON: JSON is not a string, object or purposefully null. Defaulting to no-op.");
    }

    return [](){};
}

