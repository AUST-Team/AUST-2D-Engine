#include <SDL3_mixer/SDL_mixer.h>

#include "AudioAsset.h"
#include "MemoryTracker.h"

AudioAsset::AudioAsset(MIX_Audio* audio) : 
	audio(audio) {}

AudioAsset::AudioAsset(AudioAsset&& other) noexcept :
    audio(std::exchange(other.audio, nullptr)) {}

AudioAsset::~AudioAsset()
{
    if (audio) 
    {
        ENG_MIX_DestroyAudio(audio);
        audio = nullptr;
    }
}

void AudioAsset::SetAudio(MIX_Audio* newAudio)
{
    if (audio)
    {
        ENG_MIX_DestroyAudio(audio);
    }

    audio = newAudio;
    newAudio = nullptr;
}

MIX_Audio* AudioAsset::GetAudio() const { return audio; }

AudioAsset& AudioAsset::operator=(AudioAsset&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    if (audio)
    {
		ENG_MIX_DestroyAudio(audio);
        audio = nullptr;
    }

    audio = std::exchange(other.audio, nullptr);

    return *this;
}

AudioAsset LoadAudio(const std::filesystem::path& path, MIX_Mixer* mixer)
{
    MIX_Audio* audio = ENG_MIX_LoadAudio(mixer, path.string().c_str(), false);
    if (!audio) 
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "LoadAudio: Failed loading audio for path [%s]: %s", path.string().c_str(), SDL_GetError());
    }
    return AudioAsset(audio);
}
