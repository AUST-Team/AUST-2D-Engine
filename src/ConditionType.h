#pragma once

#ifndef CONDITIONTYPE_H_
#define CONDITIONTYPE_H_

#include <string>

#include "MapUpdateFlags.h"
#include "EnumMiscs.h"

#define CONDITION_TYPE_LIST(X)                                                          \
    X(ENUM_SENTINEL_VALUE,  ConstantConfiguration::enumSentinelValue,  ConstantConfiguration::enumSentinelStringLower,    MapUpdateFlags::None)   \
    X(Interact,         1,  "interact",         MapUpdateFlags::Interact)               \
    X(Proximity,        2,  "proximity",        MapUpdateFlags::PlayerMoved)            \
    X(Stepped,          3,  "stepped",          MapUpdateFlags::PlayerMoved)            \
    X(Facing,           4,  "facing",           MapUpdateFlags::PlayerDirectionChange | MapUpdateFlags::PlayerMoved)  \
    X(Cooldown,         5,  "cooldown",         MapUpdateFlags::None)                   \
    X(Timed,            6,  "timed",            MapUpdateFlags::None)                   \
    X(MaxActivations,   7,  "maxactivations",   MapUpdateFlags::None)                   

/**
* @brief Enumeration of condition types.
*/
enum class ConditionType : SentinelEnumType
{
    /*
		ENUM_SENTINEL_VALUE,      /// Undefined type condition.
        Interact,       /// Interact type condition.
        Proximity,      /// Proximity type condition.
        Stepped,        /// Stepped on type condition.
        Facing,         /// Facing (towards / away) type condition.
		Cooldown,       /// Cooldown type condition. Should also be repeatable, since cooldown and single use would be... interesting.
		Timed,          /// Timed type condition. The trigger is active either before or after a certain time, depending on the parameters.
		MaxActivations, /// Condition that is satisfied if the trigger has been activated less than a certain number of times.
    */
#define X(name, val, str, flag) name = val,
    CONDITION_TYPE_LIST(X)
#undef X
    COUNT       /// Number of elements.
};

template <>
inline constexpr bool isSentinelEnum<ConditionType> = true;

template <>
inline constexpr bool isCountableEnum<ConditionType> = true;

/**
* @brief Returns the type of condition from a string.
*
* @param type String containing the type.
*
* @return The ConditionType from the string ("INTERACT" -> ConditionType::Interact), or ConditionType::ENUM_SENTINEL_VALUE if the string is unknown.
*/
ConditionType ConditionTypeFromString(const std::string& type);

/**
* @brief Returns the name of the condition type.
*
* @param type The condition type.
*
* @return The string containing the name of the condition type, or "[ConstantConfiguration::enumSentinelStringLower]" if type is unknown.
*/
std::string ConditionTypeToString(ConditionType type);

/**
* @brief Retursn the flags of an Condition for a particular type.
*
* Use bitwise operators for returning multiple flags.
*
* @param type Condition type of the Condition.
*
* @return The MapUpdateFlags of the Condition with the specified type.
*/
MapUpdateFlags UpdateFlagsFromConditionType(ConditionType type);

#endif // CONDITIONTYPE_H_