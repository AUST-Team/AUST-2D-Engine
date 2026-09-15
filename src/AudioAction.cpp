#include <SDL3/SDL_log.h>

#include "AudioAction.h"

#include "StringMiscs.h"

AudioAction AudioActionFromString(const std::string& action)
{
    const std::string& actionLwr = StringMiscs::ToLower(action);

#define X(name, val, str)   \
    if (actionLwr == str)   \
        return AudioAction::name;

    AUDIO_ACTION_LIST(X)

#undef X

    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioActionFromString: Unknown action type: %s. Defaulting to \"%s\"", action.c_str(), ConstantConfiguration::enumSentinelString);
	return GetSentinel<AudioAction>();
}

std::string AudioActionToString(AudioAction action)
{
    switch (action)
    {
#define X(name, val, str)   \
        case AudioAction::name: return str;

        AUDIO_ACTION_LIST(X)

#undef X

        default:
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "AudioActionToString: Unknown audio action. Defaulting to \"%s\"", ConstantConfiguration::enumSentinelStringLower);
            return ConstantConfiguration::enumSentinelStringLower;
        }
    }
}