#include <SDL3_mixer/SDL_mixer.h>

#include "AudioBus.h"

#include "AudioTrack.h"
#include "AudioPlaybackOptions.h"

AudioBus::AudioBus(const std::string& tag, MIX_Mixer* mixer) :
	tag(tag),
	mixer(mixer) {}

void AudioBus::TagTrack(AudioTrack& track) const
{
	if (!MIX_TagTrack(track.GetTrack(), tag.c_str()))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioBus.TagTrack: Failed to add track to tag [%s]: %s", tag.c_str(), SDL_GetError());
	}
}

void AudioBus::PlayAll(const AudioPlaybackOptions* opts) const
{
	if (!MIX_PlayTag(mixer, tag.c_str(), opts ? opts->GetProperties() : 0))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioBus.PlayAll: Failed to play tracks with tag [%s]: %s", tag.c_str(), SDL_GetError());
	}
}

void AudioBus::StopAll(int64_t fadeMs) const
{
	if (!MIX_StopTag(mixer, tag.c_str(), fadeMs))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioBus.StopAll: Failed to stop tracks with tag [%s]: %s", tag.c_str(), SDL_GetError());
	}
}

void AudioBus::PauseAll() const
{
	if (!MIX_PauseTag(mixer, tag.c_str()))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioBus.PauseAll: Failed to pause tracks with tag [%s]: %s", tag.c_str(), SDL_GetError());
	}
}

void AudioBus::ResumeAll() const
{
	if (!MIX_ResumeTag(mixer, tag.c_str()))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioBus.ResumeAll: Failed to resume tracks with tag [%s]: %s", tag.c_str(), SDL_GetError());
	}
}
