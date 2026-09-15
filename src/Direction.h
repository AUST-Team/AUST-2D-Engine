#pragma once

#ifndef DIRECTION_H_
#define DIRECTION_H_

#include <array>
#include <string>
#include <SDL3/SDL_rect.h>

#include "EnumMiscs.h"

#define DIRECTION_LIST(X)           \
    ENUM_SENTINEL_X_MACRO           \
    X(Up,           1,  "up")       \
    X(Down,         2,  "down")     \
    X(Left,         3,  "left")     \
    X(Right,        4,  "right")

/**
* @brief Enumeration of directions
*/
enum class Direction : SentinelEnumType
{
    /*
	ENUM_SENTINEL_VALUE,  /// Undefined direction.
    Up,         /// 'Up' direction.
    Down,       /// 'Down' direction.
    Left,       /// 'Left' direction.
    Right,      /// 'Right' direction.
    */
#define X(name, val, str) name = val,
    DIRECTION_LIST(X)
#undef X
    COUNT   /// Number of entries.
};

template <>
inline constexpr bool isSentinelEnum<Direction> = true;

template <>
inline constexpr bool isCountableEnum<Direction> = true;

constexpr std::array<SDL_Point, enumCount<Direction>> directionOffsets =    ///Array that holds the direction vector for each direction.
{
    SDL_Point {  0, -1 }, // UP
    SDL_Point {  0,  1 }, // DOWN
    SDL_Point { -1,  0 }, // LEFT
    SDL_Point {  1,  0 }, // RIGHT
};

// Defined here because constexpr doesn't like it when things aren't visible in the same file.
/**
* @brief Returns the direction vector of a given direction.
*
* @return The direction vector as SDL_Point.
*/
inline constexpr SDL_Point GetDirectionOffset(Direction dir) { return directionOffsets[ToIndex(dir)]; }

/**
* @brief Returns the direction vector of a given direction as an SDL_FPoint.
*
* @return The direction vector pair as SDL_FPoint.
*/
inline constexpr SDL_FPoint GetDirectionOffsetFloat(Direction dir)
{
    const SDL_Point& pair = directionOffsets[ToIndex(dir)];
    return SDL_FPoint {
        static_cast<float>(pair.x),
        static_cast<float>(pair.y)
    };
}

/**
 * @brief Returns the direction towards a target position.
 *
 * @param dx The horizontal distance to the target.
 * @param dy The vertical distance to the target.
 *
 * @return The direction with the greatest distance component, or Direction::ENUM_SENTINEL_VALUE if both components are zero.
 */
Direction GetDirectionTo(float dx, float dy);

/**
* @brief Returns the opposite direction of the direction given.
* 
* @param dir Given direction.
* 
* @return Opposite direction of the direction given, or Direction::ENUM_SENTINEL_VALUE if the direction is an unknown or undefined type.
*/
Direction GetOppositeDirection(Direction dir);

/**
* @brief Returns the direction contained in the string.
*
* @param directionName String containing the direction name.
*
* @return The direction if the string is valid ("Left" -> Direction::Left), or Direction::ENUM_SENTINEL_VALUE if the string is invalid.
*/
Direction DirectionFromString(const std::string& directionName);

/**
* @brief Returns the string containing the name of the direction.
*
* @param dir Direction.
*
* @return String containing the direction name (Direction::Left -> "Left"), or "[ConstantConfiguration::enumSentinelStringLower]" if the direction is undefined.
*/
std::string DirectionToString(Direction dir);

#endif // DIRECTION_H_
