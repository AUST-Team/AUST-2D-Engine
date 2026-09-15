#include <SDL3/SDL_log.h>

#include "AudioTargetType.h"

#include "StringMiscs.h"

AudioTargetType AudioTargetTypeFromString(const std::string& type)
{
    const std::string& typeLwr = StringMiscs::ToLower(type);

#define X(name, val, str)   \
    if (typeLwr == str)     \
        return AudioTargetType::name;

    AUDIO_TARGET_TYPE_LIST(X)

#undef X

    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioTargetTypeFromString: Unknown target type: %s. Defaulting to \"%s\"", type.c_str(), ConstantConfiguration::enumSentinelString);
    return GetSentinel<AudioTargetType>();
}

std::string AudioTargetTypeToString(AudioTargetType target)
{
    switch (target)
    {
#define X(name, val, str) \
        case AudioTargetType::name: return str;

        AUDIO_TARGET_TYPE_LIST(X)

#undef X

    default:
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "AudioTargetTypeToString: Unknown audio target type. Defaulting to \"%s\"", ConstantConfiguration::enumSentinelStringLower);
            return ConstantConfiguration::enumSentinelStringLower;
        }
    }
}
