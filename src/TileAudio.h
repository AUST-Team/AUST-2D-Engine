#pragma once

#ifndef TILEAUDIO_H_
#define TILEAUDIO_H_

#include "ConstantConfiguration.h"
#include "AudioPlaybackOptions.h"

/**
* @brief Structure for a tile audio.
*/
struct TileAudio
{
    HashID clipID = ConstantConfiguration::invalidHashId;   /// Hashed ID of the clip.
    AudioPlaybackOptions playbackOverride;                  /// Override of the playback, if any.
    bool hasPlaybackOverride = false;                       /// Flag if the audio has any playback override.

    /**
    * @brief Plays the audio associated with the tile.
    * 
    * Automatically applies override if it exists and the 'applyOverride' flag is 'true'.
    * 
    * @param applyOverride If the audio should be played with the tile's playback settings ('true'), or not ('false')
    */
    void PlayTileAudio(bool applyOverride = true) const;
};

#endif // TILEAUDIO_H_