#include <SDL3_mixer/SDL_mixer.h>

#include "AudioTrack.h"

#include "AudioClip.h"
#include "AudioPlaybackOptions.h"
#include "AudioSystem.h"
#include "MemoryTracker.h"

AudioTrack::AudioTrack(MIX_Track* track) : 
    track(track) {}

AudioTrack::AudioTrack(AudioTrack&& other) noexcept :
    track(std::exchange(other.track, nullptr)),
    clipId(other.clipId) {}

AudioTrack::~AudioTrack()
{
    if(track)
    {
        ENG_MIX_DestroyTrack(track);
        track = nullptr;
    }
}

void AudioTrack::SetAudio(const AudioClip& clip)
{
    if (!MIX_SetTrackAudio(track, clip.asset.GetAudio()))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioTrack.SetAudio: Failed SetTrackAudio: %s", SDL_GetError());
    }
    clipId = clip.clipID;
}

void AudioTrack::Play(const AudioPlaybackOptions* opts)
{
    if (!MIX_SetTrackGain(track, opts ? opts->GetVolume() : 1.0f))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioTrack.Play: Failed SetTrackGain: %s", SDL_GetError());
    }

    if (!MIX_PlayTrack(track, opts ? opts->GetProperties() : 0))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioTrack.Play: Failed PlayTrack: %s", SDL_GetError());
    }
}

void AudioTrack::Stop(int64_t fadeMs)
{
    int64_t fadeFrames = MIX_TrackMSToFrames(track, fadeMs);

    if (fadeFrames == -1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioTrack.Stop: TrackMSToFrames returned -1. Defaulting to no fadeout (immediate stop): %s", SDL_GetError());
        fadeFrames = 0;
    }

    if (!MIX_StopTrack(track, fadeFrames))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioTrack.Stop: Failed StopTrack: %s", SDL_GetError());
    }
}

MIX_Track* AudioTrack::GetTrack() const { return track; }

HashID AudioTrack::GetAudioID() const { return clipId; }

AudioTrack& AudioTrack::operator=(AudioTrack&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    if (track)
    {
        ENG_MIX_DestroyTrack(track);
        track = nullptr;
    }

    track = std::exchange(other.track, nullptr);
    clipId = other.clipId;

    return *this;
}

void SDLCALL TrackFinishedCallback(void* userdata, MIX_Track* track)
{
    AudioSystem* sys = static_cast<AudioSystem*>(userdata);
    sys->MarkTrackFinished(track);
}
