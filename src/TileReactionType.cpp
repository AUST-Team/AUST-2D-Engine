#include <SDL3/SDL_log.h>
#include <bit>

#include "TileReactionType.h"

#include "StringMiscs.h"

TileReactionType TileReactionTypeFromString(const std::string& type)
{
    const std::string& typeLwr = StringMiscs::ToLower(type);

#define X(name, val, str)    \
    if (typeLwr == str) \
        return TileReactionType::name;

    TILE_REACTION_LIST(X)

#undef X

    if (typeLwr == "none")
    {
        return TileReactionType::None;
    }

    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileReactionTypeFromString: String [%s] doesn't match any name. Returning \"None\".", type.c_str());
    return TileReactionType::None;
}

std::string TileReactionTypeToString(TileReactionType type)
{
    switch (type)
    {
        case TileReactionType::None: return "none";

#define X(name, val, str) \
        case TileReactionType::name: return str;

        TILE_REACTION_LIST(X)

#undef X

        default:
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileReactionTypeToString: Unknown tile reaction type. Returning \"none\"");
            return "none";
        }
    }
}

size_t TileReactionIndexFromType(TileReactionType type)
{
    TileReactionTypeUnderlying value = static_cast<TileReactionTypeUnderlying>(type);

    // Example:
    // value = 01010
    // value - 1 = 01001
    if (value == 0 || (value & (value - 1)) != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TileReactionIndexFromType: Invalid or multi-bit reaction type [%u]", value);
        return 0;
    }

	// Counts the zeros before the first set bit (1) in the binary representation of value. So Idle being 0001 -> index 0.
    return std::countr_zero(value);
}

size_t ToIndex(TileReactionType type) { return TileReactionIndexFromType(type); }
