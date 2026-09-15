#pragma once

#ifndef TRIGGERFLAGS_H_
#define TRIGGERFLAGS_H_

#include <cstdint>
#include <type_traits>

/**
* @brief Enumeration of trigger flags.
*/
enum class TriggerFlags : uint8_t
{
    None = 0,               /// No flags.
    Repeatable = 1 << 0,    /// If the trigger should be repeatable or not.
    AutoDelete = 1 << 1,    /// If the trigger should auto-delete once triggered or not.
                            /// Takes precendece over any other flag if set (trigger will be deleted if set regardless of any other flag).
    // No count possible with flags.
};

// Automatically deduces the type of the TriggerFlags.
using TriggerFlagsType = std::underlying_type_t<TriggerFlags>;

/**
* @brief Bitwise OR operator.
*/
constexpr inline TriggerFlags operator|(TriggerFlags a, TriggerFlags b)
{
    return static_cast<TriggerFlags>(
        static_cast<TriggerFlagsType>(a) |
        static_cast<TriggerFlagsType>(b)
   );
}

/**
* @brief Assign bitwise OR operator.
*/
constexpr inline TriggerFlags& operator|=(TriggerFlags& a, TriggerFlags b) { return a = a | b; }

/**
* @brief Bitwise AND operator.
*/
constexpr inline TriggerFlags operator&(TriggerFlags a, TriggerFlags b)
{
    return static_cast<TriggerFlags>(
        static_cast<TriggerFlagsType>(a) &
        static_cast<TriggerFlagsType>(b)
    );
}

/**
* @brief Assign bitwise AND operator.
*/
constexpr inline TriggerFlags& operator&=(TriggerFlags& a, TriggerFlags b) { return a = a & b; }

/**
* @brief Bitwise NOT operator.
*/
constexpr inline TriggerFlags operator~(TriggerFlags f)
{
    return static_cast<TriggerFlags>(~static_cast<TriggerFlagsType>(f));
}

/**
 * @brief Checks whether one or more flags are set in a TriggerFlags value.
 *
 * @param flags The combined set of flags to test.
 * @param test  The flag or flags to check for.
 *
 * @return 'true' if at least one of the requested flags is set in 'flags', 'false' otherwise.
 */
constexpr inline bool HasFlag(TriggerFlags flags, TriggerFlags test) { return (flags & test) != TriggerFlags::None; }

/**
 * @brief Checks whether all desired flags are in a TriggerFlags value.
 *
 * @param flags The combined set of flags to test.
 * @param test  The flags to check for.
 *
 * @return 'true' if all of the requested flags is set in 'flags', 'false' otherwise.
 */
constexpr inline bool HasAllFlags(TriggerFlags flags, TriggerFlags test) { return (flags & test) == test; }

#endif // TRIGGERFLAGS_H_