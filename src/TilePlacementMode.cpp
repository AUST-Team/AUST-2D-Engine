#include <SDL3/SDL_log.h>

#include "TilePlacementMode.h"

#include "StringMiscs.h"

TilePlacementMode TilePlacementModeFromString(const std::string& mode)
{
	const std::string& modeLwr = StringMiscs::ToLower(mode);

#define X(name, val, str)    \
    if (modeLwr == str)               \
        return TilePlacementMode::name;

	TILE_PLACEMENT_MODE_LIST(X)

#undef X

	SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TilePlacementModeFromString: Unknown type [%s]. Defaulting to \"%s\".", mode.c_str(), ConstantConfiguration::enumSentinelString);
	return GetSentinel<TilePlacementMode>();
}

std::string TilePlacementModeToString(TilePlacementMode mode)
{
    switch (mode)
    {
#define X(name, val, str)    \
        case TilePlacementMode::name: return str;

        TILE_PLACEMENT_MODE_LIST(X)

#undef X

        default:
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TilePlacementModeToString: Unknown placement mode. Defaulting to \"%s\".", ConstantConfiguration::enumSentinelStringLower);
            return ConstantConfiguration::enumSentinelStringLower;
        }
    }
}
