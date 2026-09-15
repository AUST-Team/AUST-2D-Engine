#pragma once

#ifndef AUDIOTRACK_H_
#define AUDIOTRACK_H_

#include <SDL3/SDL_properties.h>

#include "FNV1aHash.h"
#include "ConstantConfiguration.h"

struct MIX_Track;
struct AudioClip;
class AudioPlaybackOptions;

/**
* @brief Holds the pointer to the track. A mixer can have multiple tracks. An audio is played on a track.
*/
class AudioTrack 
{
private:
    MIX_Track* track = nullptr; /// Pointer to the MIX_Track. OWNED!
    HashID clipId = ConstantConfiguration::invalidHashId;   /// Hashed ID of the currently playing clip.

    // No copying allowed due to MIX_Track being uncopyable.
    AudioTrack(const AudioTrack&) = delete;
    AudioTrack& operator=(const AudioTrack&) = delete;

public:

    /**
    * @brief Constructor.
    * 
    * @param track Pointer to the track to be used. OWNED!
    */
    explicit AudioTrack(MIX_Track* track);

    /**
    * @brief Move constructor.
    */
    AudioTrack(AudioTrack&& other) noexcept;

    /**
    * @brief Destructor.
    */
    ~AudioTrack();

    /**
    * @brief Plays the set audio on the track.
    * 
    * @param opts Pointer to the playback options, if any.
    */
    void Play(const AudioPlaybackOptions* opts = nullptr);

    /**
    * @brief Stops a track.
    * 
    * @param fadeMs Optional fade-out for the track, in miliseconds. '0' if no fade-out.
    */
    void Stop(int64_t fadeMs = 0);

    /**
    * @brief Returns the track.
    * 
    * @return Pointer to the track.
    */
    MIX_Track* GetTrack() const;

    /**
    * @brief Sets the audio of the track.
    *
    * @param asset Reference to the audio clip. WILL NOT TAKE OWNERSHIP OF THE AUDIO!
    */
    void SetAudio(const AudioClip& asset);

    /**
    * @brief Returns the ID of the last audio set.
    * 
    * @return The ID of the last audio set, or 0 for invalid ID.
    */
    HashID GetAudioID() const;

    /**
    * @brief Move operator.
    */
    AudioTrack& operator=(AudioTrack&& other) noexcept;
};

/**
* @brief Callback for a track finish.
* 
* @param userdata According to the SDL3 wiki "an opaque pointer provided by the app for its personal use.".
* @param track Track that has finished.
*/
void SDLCALL TrackFinishedCallback(void* userdata, MIX_Track* track);

#endif // AUDIOTRACK_H_