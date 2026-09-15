#pragma once

#ifndef AUDIOCLIP_H_
#define AUDIOCLIP_H_

#include "AudioAsset.h"
#include "AudioPlaybackOptions.h"
#include "FNV1aHash.h"

/**
* @brief Structure for an audio clip.
* 
* This is the asset of the audio.
*/
struct AudioClip
{
    HashID clipID = ConstantConfiguration::invalidHashId;      /// ID of the clip.
    HashID busID = ConstantConfiguration::invalidHashId;       /// Bus (tag) of the clip.
    AudioAsset asset;   /// The actual asset (audio) of the clip.
    AudioPlaybackOptions defaultOptions;  /// Default options for playback.
};

#endif // AUDIOCLIP_H_