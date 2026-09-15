#pragma once

#ifndef TILEPLACEMENTMODE_H_
#define TILEPLACEMENTMODE_H_

#include <cstdint>
#include <string>

#include "EnumMiscs.h"
#include "ConstantConfiguration.h"

#define TILE_PLACEMENT_MODE_LIST(X)     \
    ENUM_SENTINEL_X_MACRO               \
    X(UseTileDefault, 1, "default")     \
    X(Replace,        2, "replace")     \
    X(Add,            3, "add")         \
    X(Remove,         4, "remove")

/**
* @brief Enumeration of tile placement modes.
*/
enum class TilePlacementMode : SentinelEnumType
{
    /*
	ENUM_SENTINEL_VALUE,    /// Placement mode undefined. Will use the default placement mode of the tile.
    UseTileDefault, /// Uses the tile's defaults, as defined in the tiles.json. This CANNOT be used in tiles.json.
    Replace,        /// Clears a map cell, then adds.
    Add,            /// Add on top existing tiles.
    Remove,         /// Removes the tile. Will remove all indexes of the respective from the cell.
    */
#define X(name, val, str) name = val,
    TILE_PLACEMENT_MODE_LIST(X)
#undef X
    COUNT,  /// Number of elements.
};

template <>
inline constexpr bool isSentinelEnum<TilePlacementMode> = true;

template <>
inline constexpr bool isCountableEnum<TilePlacementMode> = true;

/**
* @brief Returns the tile placement mode from a string.
* 
* @param mode String containing the tile placement mode.
* 
* @return The TilePlacementMode according to the string, or TilePlacementMode::ENUM_SENTINEL_VALUE if the string is unknown.
*/
TilePlacementMode TilePlacementModeFromString(const std::string& mode);

/**
* @brief Returns the name of the tile placement mode.
*
* @param mode The tile placement mode.
*
* @return The string containing the name of the placement mode, or "[ConstantConfiguration::enumSentinelStringLower]" if the mode is unknown.
*/
std::string TilePlacementModeToString(TilePlacementMode mode);

#endif // TILEPLACEMENTMODE_H_