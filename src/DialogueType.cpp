#include <SDL3/SDL_log.h>

#include "DialogueType.h"

#include "StringMiscs.h"

DialogueType DialogueTypeFromString(const std::string& type)
{
    const std::string& typeLwr = StringMiscs::ToLower(type);

#define X(name, val, str)   \
    if (typeLwr == str)     \
        return DialogueType::name;

    DIALOGUE_TYPE_LIST(X)

#undef X

    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "DialogueTypeFromString: Unknown dialogue type: %s. Defaulting to \"%s\"", type.c_str(), ConstantConfiguration::enumSentinelString);
    return GetSentinel<DialogueType>();
}

std::string DialogueTypeToString(DialogueType type)
{
    switch (type)
    {
#define X(name, val, str) \
        case DialogueType::name: return str;

        DIALOGUE_TYPE_LIST(X)

#undef X

        default:
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueTypeToString: Unknown dialogue type. Defaulting to \"%s\"", ConstantConfiguration::enumSentinelStringLower);
            return ConstantConfiguration::enumSentinelStringLower;
        }
    }
}
