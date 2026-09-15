#pragma once

#ifndef AUDIOACTION_H_
#define AUDIOACTION_H_

#include <cstdint>
#include <string>

#include "EnumMiscs.h"

#define AUDIO_ACTION_LIST(X)        \
    ENUM_SENTINEL_X_MACRO           \
    X(Play,         1,  "play")     \
    X(Stop,         2,  "stop")     \
    X(Pause,        3,  "pause")    \
    X(Resume,       4,  "resume")   \
    X(PlayOrResume, 5,  "playorresume")

/**
* @brief Enumeration for all possible audio actions.
*/
enum class AudioAction : SentinelEnumType
{
    /*
		ENUM_SENTINEL_VALUE,  /// Undefined action.
        Play,       /// Plays an audio.
        Stop,       /// Stops an audio.
        Pause,      /// Pauses an audio.
        Resume,     /// Resumes an audio.
    */
#define X(name, val, str) name = val,
    AUDIO_ACTION_LIST(X)
#undef X
    COUNT   /// Number of elements.
};

template <>
inline constexpr bool isSentinelEnum<AudioAction> = true;

template <>
inline constexpr bool isCountableEnum<AudioAction> = true;

/**
* @brief Returns the type of audio action from a string.
*
* @param action String containing the action.
*
* @return The AudioAction from the string ("PlAy" -> AudioAction::Play), or AudioAction::ENUM_SENTINEL_VALUE if the string is unknown.
*/
AudioAction AudioActionFromString(const std::string& action);

/**
* @brief Returns the name of the audio action.
*
* @param action The audio action.
*
* @return The string containing the name of the audio action, or "[ConstantConfiguration::enumSentinelStringLower]" if the action is unknown.
*/
std::string AudioActionToString(AudioAction action);

#endif // AUDIOACTION_H_