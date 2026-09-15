#pragma once

#ifndef MAPUPDATEFLAGS
#define MAPUPDATEFLAGS

#include <cstdint>
#include <type_traits>

/**
* @brief Enumation of flags for a map update;
*/
enum class MapUpdateFlags : uint8_t
{
    None = 0,                       /// No update necessary.
    PlayerMoved = 1 << 0,           /// Updates related to player movement.
    Interact = 1 << 1,              /// Updates related to player interaction.
    PlayerDirectionChange = 1 << 2, /// Updates related to the player changing the facing direction.
    // No COUNT possible with flags.
};

/// The type of the MapUpdateFlags enum.
using MapUpdateFlagsType = std::underlying_type_t<MapUpdateFlags>;

/**
* @brief Bitwise OR operator.
*/
constexpr inline MapUpdateFlags operator|(MapUpdateFlags a, MapUpdateFlags b)
{
    return static_cast<MapUpdateFlags>(
        static_cast<MapUpdateFlagsType>(a) |
        static_cast<MapUpdateFlagsType>(b)
    );
}

/**
* @brief Assign bitwise OR operator.
*/
constexpr inline MapUpdateFlags& operator|=(MapUpdateFlags& a, MapUpdateFlags b) { return a = a | b; }

/**
* @brief Bitwise AND operator.
*/
constexpr inline MapUpdateFlags operator&(MapUpdateFlags a, MapUpdateFlags b)
{
    return static_cast<MapUpdateFlags>(
        static_cast<MapUpdateFlagsType>(a) &
        static_cast<MapUpdateFlagsType>(b)
    );
}

/**
* @brief Assign bitwise AND operator.
*/
constexpr inline MapUpdateFlags& operator&=(MapUpdateFlags& a, MapUpdateFlags b) { return a = a & b; }

/**
* @brief Bitwise NOT operator.
*/
constexpr inline MapUpdateFlags operator~(MapUpdateFlags f)
{
    return static_cast<MapUpdateFlags>(~static_cast<MapUpdateFlagsType>(f));
}

/**
 * @brief Checks whether one or more flags are set in a MapUpdateFlags value.
 *
 * @param flags The combined set of flags to test.
 * @param test  The flag or flags to check for.
 *
 * @return 'true' if at least one of the requested flags is set in 'flags', 'false' otherwise.
 */
constexpr inline bool HasFlag(MapUpdateFlags flags, MapUpdateFlags test) { return (flags & test) != MapUpdateFlags::None; }

/**
* @brief Helper function that parses each flag of the given update mask.
* 
* @param flags Flags to parse.
* @param fn Function to process the flags. Auto to keep constexpr.
*/
constexpr inline void ForEachFlag(MapUpdateFlags flags, auto&& fn)
{
    MapUpdateFlagsType mask = static_cast<MapUpdateFlagsType>(flags);

    for (MapUpdateFlagsType bit = 1; bit != 0; bit <<= 1)
    {
        if (mask & bit)
        {
            fn(static_cast<MapUpdateFlags>(bit));
        }
    }
}


#endif // MAPUPDATEFLAGS