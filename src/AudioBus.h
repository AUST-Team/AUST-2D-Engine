#pragma once

#ifndef AUDIOBUS_H_
#define AUDIOBUS_H_

#include <string>

class AudioTrack;
class AudioPlaybackOptions;

/**
* @brief Bus (~track group) for audio with a certain tag. Groups them by (tag, mixer) ~ composite primary key from data bases.
*/
class AudioBus 
{
private:
    std::string tag;            /// Tag for the bus (tracks). Not hashed because SDL wants the string tag.
    MIX_Mixer* mixer = nullptr; /// Pointer to the mixer of the tracks. NON OWNING!

public:

    /**
    * @brief Constructor.
    * 
    * @param tag String containing the tag of the bus
    * @param mixer Pointer to the mixer. NON OWNING!
    */
    AudioBus(const std::string& tag, MIX_Mixer* mixer);

    /**
    * @brief Tags a track to the bus' trag.
    * 
    * @param track Reference to the track to tag. WILL NOT TAKE OWNERSHIP OF THE TRACK!
    */
    void TagTrack(AudioTrack& track) const;

    /**
    * @brief Plays all the tracks with this tag.
    * 
    * @param opts Playback options, will be applied to all tracks! Nullptr for no options.
    */
    void PlayAll(const AudioPlaybackOptions* opts = nullptr) const;

    /**
    * @brief Stops (ending the audio!) all tracks on the bus.
    * 
    * @param fadeMs Optional fade-out on audio in milliseconds (0 for immediate stop).
    */
    void StopAll(int64_t fadeMs = 0) const;

    /**
    * @brief Pauses all tracks on the bus.
    */
    void PauseAll() const;

    /**
    * @brief Resumes all tracks on the bus. 
    */
    void ResumeAll() const;
};

#endif // AUDIOBUS_H_