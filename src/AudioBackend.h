#pragma once

#ifndef AUDIOBACKEND_H_
#define AUDIOBACKEND_H_

#include <string>

struct MIX_Mixer;
struct MIX_Audio;

/**
* @brief Backend for the audio. If library ever changes, just change this.
* 
* In SDL3_mixer:
* 
* Mixer -> holds multiple tracks, is the one that 'links' to the playback device (AudioBackend here).
* Audio -> holds the data of the actual audio, played on a track (AudioAsset here).
* Track -> plays the audio, can have multiple effects added, has a tag (if any) (AudioTrack here).
* Tags  -> 'groups' of tags by name (AudioBus here).
*/
class AudioBackend
{
private:
    MIX_Mixer* mixer = nullptr; /// Pointer to the mixer. OWNED!

    // No copying allowed due to MIX_Mixer being uncopyable.
    AudioBackend(const AudioBackend&) = delete;
    AudioBackend& operator=(const AudioBackend&) = delete;

public:

    /**
    * @brief Default constructor.
    */
    AudioBackend() = default;

    /**
    * @brief Destructor.
    */
    ~AudioBackend();

    /**
    * @brief Move constructor.
    */
    AudioBackend(AudioBackend&& other) noexcept;

    /**
    * @brief Initialises the audio backend.
    * 
    * @return 'true' if initialisation was successful, 'false' if otherwise.
    */
    bool Init();
    
    /**
    * @brief Shuts down the audio backend.
    */
    void Shutdown();

    /**
    * @brief Returns the mixer.
    * 
    * @return Pointer to the mixer.
    */
    MIX_Mixer* GetMixer() const;
    
    /**
    * @brief Move operator.
    */
    AudioBackend& operator=(AudioBackend&& other) noexcept;
};

#endif // AUDIOBACKEND_H_