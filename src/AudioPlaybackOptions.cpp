#include <SDL3/SDL_log.h>
#include <SDL3_mixer/SDL_mixer.h>

#include "AudioPlaybackOptions.h"

#include "FloatUtils.h"
#include "MemoryTracker.h"

AudioPlaybackOptions::AudioPlaybackOptions() : 
	properties(ENG_SDL_CreateProperties())
{
	if (!properties)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioPlaybackOptions.AudioPlaybackOptions: Failed CreateProperties: %s", SDL_GetError());
	}
}

AudioPlaybackOptions::AudioPlaybackOptions(const AudioPlaybackOptions& other) :
	properties(ENG_SDL_CreateProperties())
{
	if (!properties)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioPlaybackOptions: Failed CreateProperties: %s", SDL_GetError());
		return;
	}

	if (!SDL_CopyProperties(other.properties, properties))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioPlaybackOptions: Failed SDL_CopyProperties: %s", SDL_GetError());
		ENG_SDL_DestroyProperties(properties);
		properties = 0;
	}

	volume = other.volume;
}

AudioPlaybackOptions::AudioPlaybackOptions(AudioPlaybackOptions&& other) noexcept : 
	properties(other.properties),
	volume(other.volume) { other.properties = 0; }

AudioPlaybackOptions::~AudioPlaybackOptions()
{
	if (properties)
	{
		ENG_SDL_DestroyProperties(properties);
		properties = 0;
	}
}

void AudioPlaybackOptions::SetLoopCount(int64_t nrLoops) const
{
	const bool success = SDL_SetNumberProperty(
		properties,
		MIX_PROP_PLAY_LOOPS_NUMBER,
		nrLoops
	);

	if (!success)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioPlaybackOptions.SetLooping: Failed SetNumberProperty: %s", SDL_GetError());
	}
}

void AudioPlaybackOptions::SetFadeInMs(int64_t fadeMs) const
{
	const bool success = SDL_SetNumberProperty(
		properties,
		MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER,
		fadeMs
	);

	if (!success)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioPlaybackOptions.SetFadeInMs: Failed SetNumberProperty: %s", SDL_GetError());
	}
}

void AudioPlaybackOptions::SetVolume(float newVolume) 
{ 
	volume = FloatUtils::IsLessF(newVolume, 0.0f) ? 0.0f : newVolume;
}

float AudioPlaybackOptions::GetVolume() const { return volume; }

SDL_PropertiesID AudioPlaybackOptions::GetProperties() const { return properties; }

AudioPlaybackOptions& AudioPlaybackOptions::operator=(const AudioPlaybackOptions& other)
{
	if (this == &other)
	{
		return *this;
	}

	SDL_PropertiesID newProps = ENG_SDL_CreateProperties();
	if (!newProps)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioPlaybackOptions.CopyOperator: Failed CreateProperties: %s", SDL_GetError());
		return *this;
	}

	if (!SDL_CopyProperties(other.properties, newProps))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioPlaybackOptions.CopyOperator: Failed CopyProperties: %s", SDL_GetError());
		ENG_SDL_DestroyProperties(newProps);
		return *this;
	}

	if(properties)
	{
		ENG_SDL_DestroyProperties(properties);
		properties = 0;
	}

	properties = std::exchange(newProps, 0);
	volume = other.volume;

	return *this;
}

AudioPlaybackOptions& AudioPlaybackOptions::operator=(AudioPlaybackOptions&& other) noexcept
{
	if (this == &other)
	{
		return *this;
	}

	if (properties)
	{
		ENG_SDL_DestroyProperties(properties);
		properties = 0;
	}

	properties = std::exchange(other.properties, 0);
	volume = other.volume;

	return *this;
}
