#pragma once

#ifndef DIALOGUETYPE_H_
#define DIALOGUETYPE_H_

#include <string>

#include "EnumMiscs.h"

#define DIALOGUE_TYPE_LIST(X)			\
    ENUM_SENTINEL_X_MACRO				\
    X(Narration,	1,  "narration")	\
    X(Choice,       2,  "choice")       \
    X(Node,         3,  "node")        


/**
* @brief Enumeration of dialogue types.
*/
enum class DialogueType : SentinelEnumType
{
    /*
        SENTINEL    /// Sentinel value (undefined).
        Narration   /// Narration dialogue.
        Choice      /// Choice dialogue.
    */
#define X(name, val, str) name = val,
    DIALOGUE_TYPE_LIST(X)
#undef X
    COUNT   /// Number of elements.
};

template <>
inline constexpr bool isSentinelEnum<DialogueType> = true;

template <>
inline constexpr bool isCountableEnum<DialogueType> = true;

/**
* @brief Parses and returns the DialogueType from a string.
* 
* @param type String containing the type.
* 
* @return The corresponding DialogueType, or T::ENUM_SENTINEL_VALUE if nothing matches.
*/
DialogueType DialogueTypeFromString(const std::string& type);

/**
* @brief Returns the string associated with a dialogue type.
* 
* @param type The type of dialogue.
* 
* @return The string containing the name of the dialogue type, or "[ConstantConfiguration::enumSentinelStringLower]" if the type is invalid.
*/
std::string DialogueTypeToString(DialogueType type);

#endif // DIALOGUETYPE_H_