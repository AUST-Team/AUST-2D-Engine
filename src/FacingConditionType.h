#pragma once

#ifndef FACINGCONDITIONTYPE_H_
#define FACINGCONDITIONTYPE_H_

#include <string>

#include "EnumMiscs.h"

#define FACING_CONDITION_TYPE_LIST(X)       \
    X(ENUM_SENTINEL_VALUE, ConstantConfiguration::enumSentinelValue, ConstantConfiguration::enumSentinelStringLower,    "")     \
    X(LookAt,    1, "lookat",       "at")   \
    X(LookAway,  2, "lookaway",     "away")

/**
* @brief Enumeration of facing condition types.
*/
enum class FacingConditionType : SentinelEnumType
{
    /*
	ENUM_SENTINEL_VALUE,  /// Undefined facing condition type.
    LookAt,     /// Facing towards the condition (tile).
    LookAway,   /// Facing away from the condition (tile).
    */
#define X(name, val, primary, alias) name = val,
    FACING_CONDITION_TYPE_LIST(X)
#undef X
    COUNT       /// Number of elements.
};

template <>
inline constexpr bool isSentinelEnum<FacingConditionType> = true;

template <>
inline constexpr bool isCountableEnum<FacingConditionType> = true;

/**
* @brief Returns the facing type from a string.
* 
* @param type String of the type.
* 
* @return The FacingConditionType from the respective string ("AT" / "LookAt" -> LookAt), or FacingConditionType::ENUM_SENTINEL_VALUE if the string is unknown.
*/
FacingConditionType FacingConditionTypeFromString(const std::string& type);

/**
* @brief Returns the string of the facing condition type.
* 
* @param type Type of facing condition.
* 
* @return The string of the facing condition type (LookAt -> "lookat"), or "[ConstantConfiguration::enumSentinelStringLower]" if the type is unknown.
*/
std::string FacingConditionTypeToString(FacingConditionType type);

#endif // FACINGCONDITIONTYPE_H_