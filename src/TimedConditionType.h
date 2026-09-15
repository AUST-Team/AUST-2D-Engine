#pragma once

#ifndef TIMEDCONDITIONTYPE_H_
#define TIMEDCONDITIONTYPE_H_

#include <string>

#include "EnumMiscs.h"

#define TIMED_CONDITION_TYPE_LIST(X)    \
    ENUM_SENTINEL_X_MACRO               \
    X(Before,       1,  "before")       \
    X(After,        2,  "after")        \
    X(Specific,     3,  "specific")     \
    X(Interval,     4,  "interval")             

/**
* @brief Enumeration of timed condition types.
*/
enum class TimedConditionType : SentinelEnumType
{
    /*
	ENUM_SENTINEL_VALUE,  /// Condition is not defined (sentinel value).
	Before,     /// Condition is satisfied if the current time is below the specified time.
	After,      /// Condition is satisfied if the current time is above the specified time.
	Specific,   /// Condition is satisfied if the current time is equal to the specified time.
	Interval    /// Condition is satisfied if the current time is within the specified interval.
    */
#define X(name, val, str) name = val,
    TIMED_CONDITION_TYPE_LIST(X)
#undef X
    COUNT       /// Number of elements.
};

template <>
inline constexpr bool isSentinelEnum<TimedConditionType> = true;

template <>
inline constexpr bool isCountableEnum<TimedConditionType> = true;

/**
* @brief Returns the timed condition type from a string.
*
* @param type String of the type.
*
* @return The TimedConditionType from the respective string ("BEfore" -> Before), or TimedConditionType::ENUM_SENTINEL_VALUE if the string is unknown.
*/
TimedConditionType TimedConditionTypeFromString(const std::string& type);

/**
* @brief Returns the name of the timed condition type.
*
* @param type The timed condition type.
*
* @return The string containing the name of the timed condition type, or "[ConstantConfiguration::enumSentinelStringLower]" if the type is unknown.
*/
std::string TimedConditionTypeToString(TimedConditionType type);

#endif // TIMEDCONDITIONTYPE_H_