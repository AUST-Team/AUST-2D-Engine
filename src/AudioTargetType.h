#pragma once

#ifndef AUDIOTARGETTYPE_H_
#define AUDIOTARGETTYPE_H_

#include <cstdint>
#include <string>

#include "EnumMiscs.h"

#define AUDIO_TARGET_TYPE_LIST(X)	\
	ENUM_SENTINEL_X_MACRO			\
    X(Clip,			1,	"clip")     \
    X(Tag,			2,  "tag")      \
    X(All,			3,	"all")

/**
* @brief Target type of an audio.
*/
enum class AudioTargetType : SentinelEnumType
{
	/*
	ENUM_SENTINEL_VALUE,	/// Target is undefined.
	Clip,		/// Target is a specific clip.
	Tag,		/// Target is a specific tag.
	All			/// Target is all clips.
	*/
#define X(name, val, str) name = val,
	AUDIO_TARGET_TYPE_LIST(X)
#undef X
	COUNT	/// Number of elements.
};

template <>
inline constexpr bool isSentinelEnum<AudioTargetType> = true;

template <>
inline constexpr bool isCountableEnum<AudioTargetType> = true;

/**
*@brief Returns the type of audio target type from a string.
*
* @param type String containing the type.
*
* @return The AudioTargetType from the string("Clip"->AudioAction::Clip), or AudioTargetType::ENUM_SENTINEL_VALUE if the string is invalid.
*/
AudioTargetType AudioTargetTypeFromString(const std::string& type);

/**
* @brief Returns the name of the audio target type.
*
* @param target The audio target type.
*
* @return The string containing the name of the audio target type, or "[ConstantConfiguration::enumSentinelStringLower]" if the type is invalid.
*/
std::string AudioTargetTypeToString(AudioTargetType target);

#endif // AUDIOTARGETTYPE_H_