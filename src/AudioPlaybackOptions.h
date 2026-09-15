#pragma once

#ifndef AUDIOPLAYBACKOPTIONS_H_
#define AUDIOPLAYBACKOPTIONS_H_

#include <SDL3/SDL_properties.h>
#include <nlohmann/json.hpp>

/**
* @brief Wrapper class for playback options using SDL3's SDL_PropertiesID.
* 
* I'm still not entirely sure how to handle them, the documentation is a bit... lacking in clear what I'm supposed to do to put 'loop audio' in them.
* 
* Apparently they're handlers for options, so treat them [SDL_PropertiesID] like pointers.
*/
class AudioPlaybackOptions
{
private:
    SDL_PropertiesID properties = 0;    /// Properties (options) of the playback. 0 is 'none'.
    float volume = 1.0f;    /// Volume of the playback. 1.0 for '100%' volume, 0.0 for muted.

public:

    /**
    * @brief Constructor.
    */
    AudioPlaybackOptions();

    /**
    * @brief Copy operator.
    */
    AudioPlaybackOptions(const AudioPlaybackOptions& other);

    /**
    * @brief Move operator.
    */
    AudioPlaybackOptions(AudioPlaybackOptions&& other) noexcept;

    /**
    * @brief Destructor.
    */
    ~AudioPlaybackOptions();

    /**
    * @brief Sets the number of loops on the playback options.
    *
    * @param nrLoops Number of loops to do then stop. -1 for infinite loops, 0 to disable looping (if set to loop).
    */
    void SetLoopCount(int64_t nrLoops = -1) const;

    /**
    * @brief Sets the fade in time in ms on the playback options.
    *
    * @param fadeMs The number of milliseconds for the fade-in.
    */
    void SetFadeInMs(int64_t fadeMs) const;

    /**
    * @brief Sets the volume of the playback.
    * 
    * @param newVolume New volume of the playback.
    */
    void SetVolume(float newVolume);

    /**
    * @brief Returns the volume of the playback.
    * 
    * @return The volume of the playback.
    */
    float GetVolume() const;

    /**
    * @brief Returns the properties of the playback options.
    * 
    * @return Properties of the playback options.
    */
    SDL_PropertiesID GetProperties() const;

    /**
    * @brief Copy operator.
    */
    AudioPlaybackOptions& operator=(const AudioPlaybackOptions& other);

    /**
    * @brief Move operator.
    */
    AudioPlaybackOptions& operator=(AudioPlaybackOptions&& other) noexcept;
};

#endif // AUDIOPLAYBACKOPTIONS_H_