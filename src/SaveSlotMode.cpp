#include <SDL3/SDL_log.h>

#include "SaveSlotMode.h"
#include "StringMiscs.h"

SaveSlotMode SaveSlotModeFromString(const std::string& mode)
{
    const std::string& modeLwr = StringMiscs::ToLower(mode);

#define X(name, val, str)    \
    if (modeLwr == str) \
        return SaveSlotMode::name;

    SAVE_SLOT_MODE_LIST(X)

#undef X

    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SaveSlotModeFromString: Unknown mode [%s]. Defaulting to \"%s\"", mode.c_str(), ConstantConfiguration::enumSentinelString);
	return GetSentinel<SaveSlotMode>();
}

std::string SaveSlotModeToString(SaveSlotMode mode)
{
    switch (mode)
    {
#define X(name, val, str)   \
        case SaveSlotMode::name: return str;

        SAVE_SLOT_MODE_LIST(X)

#undef X

    default:
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SaveSlotModeToString: Unknown save slot mode. Defaulting to \"%s\"", ConstantConfiguration::enumSentinelStringLower);
            return ConstantConfiguration::enumSentinelStringLower;
        }
    }
}