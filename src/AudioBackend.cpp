#include <SDL3_mixer/SDL_mixer.h>

#include "AudioBackend.h"
#include "AudioSystem.h"
#include "MemoryTracker.h"

AudioBackend::~AudioBackend() { Shutdown(); }

AudioBackend::AudioBackend(AudioBackend&& other) noexcept :
    mixer(std::exchange(other.mixer, nullptr)) {}

bool AudioBackend::Init()
{
    if (!MIX_Init()) 
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "AudioBackend.Init: Failed to initialise SDL mix: %s", SDL_GetError());
        return false;
    }

    mixer = ENG_MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!mixer) 
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "AudioBackend.Init: Failed to create mixer device: %s", SDL_GetError());
        return false;
    }

    return true;
}

void AudioBackend::Shutdown() 
{
    if (mixer) 
    {
        ENG_MIX_DestroyMixer(mixer);
        mixer = nullptr;
    }
    MIX_Quit();
}

MIX_Mixer* AudioBackend::GetMixer() const { return mixer; }

AudioBackend& AudioBackend::operator=(AudioBackend&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    if (mixer)
    {
        ENG_MIX_DestroyMixer(mixer);
        mixer = nullptr;
    }

    mixer = std::exchange(other.mixer, nullptr);

    return *this;
}
