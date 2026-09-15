#pragma once

#ifndef TILEREACTIONTYPE_H_
#define TILEREACTIONTYPE_H_

#include <cstdint>
#include <optional>
#include <string>

// Scream if you love macros.
#define TILE_REACTION_LIST(X)           \
    X(Idle,         0,  "idle")         \
    X(SteppedOn,    1,  "steppedon")    \
    X(LookAt,       2,  "lookat")       \
    X(LookAway,     3,  "lookaway")     \
    X(Walk,         4,  "walk")         \
    X(Attack,       5,  "attack")       \
    X(Death,        6,  "death")

/// The type of the TileReactionType enum.
using TileReactionTypeUnderlying = uint8_t;

/**
* @brief Index of each reaction type.
*/
enum TileReactionIndex : TileReactionTypeUnderlying
{
#define X(name, val, str) name = val,
    TILE_REACTION_LIST(X)
#undef X
    COUNT   /// Number of elements.
};

/**
* @brief Enumeration of reaction types.
*/
enum class TileReactionType : TileReactionTypeUnderlying
{
    None = 0,           /// No animation.
    // No VS2022, I don't know what a 'browsing operation' is, and I will not add a hint.
#define X(name, val, str) name = 1 << val,
    TILE_REACTION_LIST(X)
#undef X
    /*
        Idle = 1 << 0,      /// Idle animation.
        SteppedOn = 1 << 1, /// Stepped on.
        LookAt = 1 << 2,    /// Look at animation.
        LookAway = 1 << 3,  /// Look away animation.
        Walk = 1 << 4,      /// Walk animation.
        Attack = 1 << 5,    /// Attack animation.
        Death = 1 << 6      /// Death animation.
        // No number of elements with flag enums.
    */
};

/// Aliases. Although they're both the same, a reaction type can have either an animation, sound, or both.
using TileAnimationType = TileReactionType;
using TileSoundType = TileReactionType;

/// Count of animations (not including None).
constexpr size_t TileReactionCount = static_cast<size_t>(TileReactionIndex::COUNT);

/**
* @brief Bitwise OR operator.
*/
constexpr inline TileReactionType operator|(TileReactionType a, TileReactionType b)
{
    return static_cast<TileReactionType>(
        static_cast<TileReactionTypeUnderlying>(a) |
        static_cast<TileReactionTypeUnderlying>(b)
    );
}

/**
* @brief Assign bitwise OR operator.
*/
constexpr inline TileReactionType& operator|=(TileReactionType& a, TileReactionType b) { return a = a | b; }

/**
* @brief Bitwise AND operator.
*/
constexpr inline TileReactionType operator&(TileReactionType a, TileReactionType b)
{
    return static_cast<TileReactionType>(
        static_cast<TileReactionTypeUnderlying>(a) &
        static_cast<TileReactionTypeUnderlying>(b)
    );
}

/**
* @brief Assign bitwise AND operator.
*/
constexpr inline TileReactionType& operator&=(TileReactionType& a, TileReactionType b) { return a = a & b; }

/**
* @brief Bitwise NOT operator.
*/
constexpr inline TileReactionType operator~(TileReactionType f)
{
    return static_cast<TileReactionType>(~static_cast<TileReactionTypeUnderlying>(f));
}

/**
 * @brief Checks whether one or more flags are set in a TileReactionType value.
 *
 * @param flags The combined set of flags to test.
 * @param test  The flag or flags to check for.
 *
 * @return 'true' if at least one of the requested flags is set in 'flags', 'false' otherwise.
 */
constexpr inline bool HasFlag(TileReactionType flags, TileReactionType test) { return (flags & test) != TileReactionType::None; }

/**
* @brief Checks if the flag is 'None'.
* 
* @param flag Flag to test.
* 
* @return 'true' if the flag == 'None', 'false' if otherwise.
*/
constexpr inline bool IsNone(TileReactionType flag) { return flag == TileReactionType::None; }

/**
* @brief Returns the reaction type of a string.
* 
* @param type String containing the name of the reaction type.
* 
* @return The reaction type, or ::None if the string is unknown.
*/
TileReactionType TileReactionTypeFromString(const std::string& type);

/**
* @brief Returns the name of the tile reaction type.
*
* @param type The tile reaction type.
*
* @return The string containing the name of the tile reaction type, or "None" if the type is unknown.
*/
std::string TileReactionTypeToString(TileReactionType type);

/**
* @brief Returns the index of an reaction type.
* 
* @param type The reaction type.
* 
* @return The index of the reaction type.
*/
size_t TileReactionIndexFromType(TileReactionType type);

/**
* @brief Returns the index of an reaction type.
*
* Identical to TileReactionIndexFromType, but with a shorter name.
* 
* @param type The reaction type.
*
* @return The index of the reaction type.
*/
size_t ToIndex(TileReactionType type);

#endif // TILEREACTIONTYPE_H_