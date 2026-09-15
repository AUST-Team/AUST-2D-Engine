#include "TileAudio.h"

#include "Audio.h"

void TileAudio::PlayTileAudio(bool applyOverride) const
{
	if (hasPlaybackOverride && applyOverride)
	{
		Audio::Play(clipID, playbackOverride);
	}
	else
	{
		Audio::Play(clipID, nullptr);
	}
}
