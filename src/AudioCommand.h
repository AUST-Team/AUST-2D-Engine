#pragma once

#ifndef AUDIOCOMMAND_H_
#define AUDIOCOMMAND_H_

#include "AudioCommandOptions.h"
#include "ConstantConfiguration.h"
#include "AudioAction.h"
#include "AudioTargetType.h"

/**
* @brief An audio command.
*/
struct AudioCommand
{
    AudioCommandOptions opts;   /// Command options (playback options, fade out, etc).
    AudioAction action = AudioAction::Play; /// Action type.
    AudioTargetType targetType = AudioTargetType::Clip; /// Action target.
    HashID targetID = ConstantConfiguration::invalidHashId; /// Target ID.
};

#endif // AUDIOCOMMAND_H_