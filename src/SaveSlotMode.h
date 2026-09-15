#pragma once

#ifndef SAVEPANELMODE_H_
#define SAVEPANELMODE_H_

#include <cstdint>
#include <string>

#include "EnumMiscs.h"

#define SAVE_SLOT_MODE_LIST(X)  \
    ENUM_SENTINEL_X_MACRO       \
    X(Load, 1,  "load")         \
    X(Save, 2,  "save")

/**
 * @brief Panel mode for the load / save panel.
 */
enum class SaveSlotMode : SentinelEnumType
{
    /*
		ENUM_SENTINEL_VALUE,  /// Undefined save slot mode.
        Load,    /// Specifies to create a load panel.
        Save,    /// Specifies to create a save panel.
    */
#define X(name, val, str) name,
    SAVE_SLOT_MODE_LIST(X)
#undef X
    COUNT    /// Number of elements.
};

template <>
inline constexpr bool isSentinelEnum<SaveSlotMode> = true;

template <>
inline constexpr bool isCountableEnum<SaveSlotMode> = true;

/**
 * @brief Returns the save slot mode from a string.
 *
 * @param mode String containing the mode name.
 *
 * @return The SaveSlotMode according to the string ("load" -> SaveSlotMode::Load), or SaveSlotMode::ENUM_SENTINEL_VALUE if nothing matches.
 */
SaveSlotMode SaveSlotModeFromString(const std::string& mode);

/**
 * @brief Returns the name of the save slot mode.
 *
 * @param mode The save slot mode.
 *
 * @return The string containing the name of the save slot mode, or "[ConstantConfiguration::enumSentinelStringLower]" if mode is unknown.
 */
std::string SaveSlotModeToString(SaveSlotMode mode);

#endif // SAVEPANELMODE_H_
