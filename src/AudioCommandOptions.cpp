#include "AudioCommandOptions.h"

AudioCommandOptions::AudioCommandOptions(const AudioPlaybackOptions& playbackOverride) :
	playbackOverride(playbackOverride) {}

AudioCommandOptions::AudioCommandOptions(int64_t fadeMs) :
	fadeMs(fadeMs) {}
