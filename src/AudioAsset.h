#pragma once


#ifndef AUDIOASSET_H_
#define AUDIOASSET_H_

#include <string>
#include <filesystem>

struct MIX_Audio;
struct MIX_Mixer;

/**
* @brief Audio asset, holds the pointer to the audio.
*/
class AudioAsset 
{
private:
    MIX_Audio* audio = nullptr; /// Pointer to the audio. OWNED!

    // No copying allowed due to MIX_Audio being uncopyable.
    AudioAsset(const AudioAsset&) = delete;
    AudioAsset& operator=(const AudioAsset&) = delete;

public:

    /**
    * @brief Constructor.
    * 
    * @param audio Pointer to the audio used. OWNED!
    */
    explicit AudioAsset(MIX_Audio* audio = nullptr);
    
    /**
    * @brief Move constructor.
    */
    AudioAsset(AudioAsset&& other) noexcept;

    /**
    * @brief Destructor.
    */
    ~AudioAsset();

    /**
    * @brief Sets the new audio for the asset.
    * 
    * @param newAudio Pointer to the new MIX_Audio. OWNS IT!
    */
    void SetAudio(MIX_Audio* newAudio);

    /**
    * @brief Returns the audio of the asset.
    * 
    * @return Pointer to the MIX_Audio.
    */
    MIX_Audio* GetAudio() const;

    /**
    * @brief Move operator.
    */
    AudioAsset& operator=(AudioAsset&& other) noexcept;
};

/**
* @brief Loads an audio.
* 
* @param path Path to the audio file.
* @param mixer Pointer to the mixer used.
* 
* @return The audio asset (usable audio) of the file. The pointer to the MIX_Audio in said asset will be NULL if there was a problem loading it.
*/
AudioAsset LoadAudio(const std::filesystem::path& path, MIX_Mixer* mixer);

AudioAsset LoadAudio(const std::string& path, MIX_Mixer* mixer) = delete;
AudioAsset LoadAudio(const char * path, MIX_Mixer* mixer) = delete;

#endif // AUDIOASSET_H_