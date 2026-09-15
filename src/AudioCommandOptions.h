#pragma once

#ifndef AUDIOCOMMANDOPTIONS_H_
#define AUDIOCOMMANDOPTIONS_H_

#include <cstdint>
#include <functional>

#include "AudioPlaybackOptions.h"

/**
* @brief Options for an audio command.
*/
struct AudioCommandOptions
{
	AudioPlaybackOptions playbackOverride;	/// Playback override.
	int64_t fadeMs = 0;	/// Fade out in miliseconds. 0 for no fadeout.

	/**
	* @brief Default constructor.
	*/
	AudioCommandOptions() = default;

	/**
	* @brief Constructor.
	* 
	* @param playbackOverride Playback override.
	*/
	AudioCommandOptions(const AudioPlaybackOptions& playbackOverride);

	/**
	* @brief Constructor.
	*
	* @param fadeMs Fade out in miliseconds. 0 for no fadeout.
	*/
	AudioCommandOptions(int64_t fadeMs);
};

#endif //AUDIOCOMMANDOPTIONS_H_