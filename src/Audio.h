#pragma once

#ifndef AUDIO_H_
#define AUDIO_H_

#include <functional>
#include <string>

#include "FNV1aHash.h"

class AudioPlaybackOptions;
struct AudioCommand;

/**
* @brief Namespace to play audio with. Facade for the audio system.
* 
* So this goes like this:
* AudioSystem -> initialises the backend -> gets the mixer and all SDL things initialised.
* AudioSystem -> loads the 'default' bus and all audios from a JSON file.
* Whatever system -> Audio::Play(...)
* 
* AudioAsset -> MIX_Audio* and HashID (immutable)
* AudioTrack -> MIX_Track* (immutable) and HashID (mutable, of the last AudioAsset set).
* AudioClip  -> AudioAsset, busID and default playback options.
* AudioPlaybackOptions -> Playback options.
* AudioBus   -> Busses (groups) of tracks.
* AudioBackend -> MIX_Mixer* (immutable) and SDL initialisation.
* AudioSystem -> Holds AudioBackend, singleton, holds all buses (map), clips (map), tracks (vector), cleanups finished tracks, creates new tracks
*/
namespace Audio
{
    /**
    * @brief Plays an audio.
    * 
    * A particular clip may be played multiple times simultaneously.
    * 
    * @param clipName The name of the audio to play.
    * @param override Optional override for playback options.
    */
    void Play(const std::string& clipName, std::function<void(AudioPlaybackOptions&)> override = nullptr);

    /**
    * @brief Plays an audio.
    *
    * A particular clip may be played multiple times simultaneously.
    *
    * @param clipName The name of the audio to play.
    * @param override Optional override for playback options.
    */
    void Play(const std::string& clipName, const AudioPlaybackOptions& override);

    /**
    * @brief Stops an audio. Will stop all instances of that audio!
    * 
    * If the clip is not playing, has no effect.
    * 
    * @param clipName The name of the audio currently playing.
    * @param fadeMs The fade out (if any) of the audio. 0 for immediate stop.
    */
    void Stop(const std::string& clipName, int64_t fadeMs = 0);

    /**
    * @brief Pauses an audio. Will Pauses all instances of that audio!
    *
    * If the clip is not playing, has no effect.
    * 
    * @param clipName The name of the audio currently playing.
    */
    void Pause(const std::string& clipName);

    /**
    * @brief Resumes a paused audio. Will Resumes all instances of that audio!
    * 
    * If the clip is not paused, has no effect.
    *
    * @param clipName The name of the audio currently paused.
    */
    void Resume(const std::string& clipName);

    /**
    * @brief Plays all audio clips assigned to a specific tag.
    *
    * @param tag The tag the group of audio clips to play.
    * @param override Optional override for playback options.
    */
    void PlayTag(const std::string& tag, std::function<void(AudioPlaybackOptions&)> override = nullptr);

    /**
    * @brief Plays all audio clips assigned to a specific tag.
    *
    * @param tag The tag the group of audio clips to play.
    * @param override Optional override for playback options.
    */
    void PlayTag(const std::string& tag, const AudioPlaybackOptions& override);

    /**
     * @brief Stops all currently playing audio tracks assigned to a tag.
     *
     * @param tag The tag the group of audio tracks to stop.
     * @param fadeMs Fade out in milliseconds. Use 0 for an immediate stop.
     */
    void StopTag(const std::string& tag, int64_t fadeMs = 0);

    /**
     * @brief Pauses all currently playing audio tracks assigned to a tag.
     *
     * @param tag The tag the group of audio tracks to pause.
     */
    void PauseTag(const std::string& tag);

    /**
     * @brief Resumes all paused audio tracks assigned to a tag.
     *
     * @param tag The tag identifying the group of audio tracks to resume.
     */
    void ResumeTag(const std::string& tag);

    /**
     * @brief Stops all currently playing audio tracks.
     *
     * This affects all clips and all tags.
     * 
     * @param fadeMs Optional fade out in milliseconds. 0 for immediate stop.
     */
    void StopAll(int64_t fadeMs = 0);

    /**
     * @brief Pauses all currently playing audio tracks.
     *
     * All paused tracks may later be resumed using ResumeAll().
     */
    void PauseAll();

    /**
     * @brief Resumes all paused audio tracks.
     */
    void ResumeAll();

    /**
     * @brief Plays an audio clip.
     *
     * A particular clip may be played multiple times simultaneously.
     *
     * @param clipID The ID of the audio clip to play.
     * @param override Optional override for playback options.
     */
    void Play(HashID clipID, std::function<void(AudioPlaybackOptions&)> override = nullptr);

    /**
     * @brief Plays an audio clip.
     *
     * A particular clip may be played multiple times simultaneously.
     *
     * @param clipID The ID of the audio clip to play.
     * @param override Optional override for playback options.
     */
    void Play(HashID clipID, const AudioPlaybackOptions& override);

    /**
     * @brief Stops an audio clip. Will stop all instances of that clip!
     *
     * If the clip is not playing, this has no effect.
     *
     * @param clipID The ID of the audio clip currently playing.
     * @param fadeMs The fade out (if any) of the audio. 0 for immediate stop.
     */
    void Stop(HashID clipID, int64_t fadeMs = 0);

    /**
     * @brief Pauses an audio clip. Will pause all instances of that clip!
     *
     * If the clip is not playing, this has no effect.
     *
     * @param clipID The ID of the audio clip currently playing.
     */
    void Pause(HashID clipID);

    /**
     * @brief Resumes a paused audio clip. Will resume all instances of that clip!
     *
     * If the clip is not paused, this has no effect.
     *
     * @param clipID The ID of the audio clip currently paused.
     */
    void Resume(HashID clipID);

    /**
     * @brief Plays all audio clips assigned to a specific tag.
     *
     * @param tagID The ID of the tag representing a group of audio clips to play.
     * @param override Optional override for playback options.
     */
    void PlayTag(HashID tagID, std::function<void(AudioPlaybackOptions&)> override = nullptr);

    /**
     * @brief Plays all audio clips assigned to a specific tag.
     *
     * @param tagID The ID of the tag representing a group of audio clips to play.
     * @param override Optional override for playback options.
     */
    void PlayTag(HashID tagID, const AudioPlaybackOptions& override);

    /**
     * @brief Stops all currently playing audio clips assigned to a tag.
     *
     * @param tagID The ID of the tag representing the group of audio clips to stop.
     * @param fadeMs Fade out in milliseconds. Use 0 for an immediate stop.
     */
    void StopTag(HashID tagID, int64_t fadeMs = 0);

    /**
     * @brief Pauses all currently playing audio clips assigned to a tag.
     *
     * @param tagID The ID of the tag representing the group of audio clips to pause.
     */
    void PauseTag(HashID tagID);

    /**
     * @brief Resumes all paused audio clips assigned to a tag.
     *
     * @param tagID The ID of the tag representing the group of audio clips to resume.
     */
    void ResumeTag(HashID tagID);

    /**
    * @brief Executes an general audio command.
    * 
    * @param command Audio command to be executed.
    */
    void ExecuteAudioCommand(const AudioCommand& command);

    /**
    * @brief Executes a clip audio command.
    * 
    * Recommended to use ExecuteAudioCommand or use the 'direct' API.
    * 
    * @param command Audio command to be executed.
    */
    void ExecuteClipAudioCommand(const AudioCommand& command);

    /**
    * @brief Executes a tag audio command.
    *
    * Recommended to use ExecuteAudioCommand or use the 'direct' API.
    * 
    * @param command Audio command to be executed.
    */
    void ExecuteTagAudioCommand(const AudioCommand& command);

    /**
    * @brief Executes a global audio command.
    *
    * Recommended to use ExecuteAudioCommand or use the 'direct' API.
    *
    * @param command Audio command to be executed.
    */
    void ExecuteGlobalAudioCommand(const AudioCommand& command);

    /**
    * @brief Returns the master volume of the audio system.
    * 
    * @return The master volume, between 1.0f and 0.0f, where 1.0f is 'full volume', 0.0f is 'mute'.
    */
    float GetMasterVolume();

    /**
    * @brief Sets the master volume of the audio system.
    *
    * @param newVolue The new master volume, between 1.0f and 0.0f, where 1.0f is 'full volume', 0.0f is 'mute'.
    */
    void SetMasterVolume(float newVolume);

    /**
    * @brief Checks if a clip is currently active, either being played or paused.
    *
    * @param clipName Name of the clip.
    *
    * @return 'true' if the clip is active (played, paused), or 'false' otherwise.
    */
    bool IsClipActive(const std::string& clipName);

    /**
    * @brief Checks if a clip is currently active, either being played or paused.
    *
    * @param clipHash Hash of the clip.
    *
    * @return 'true' if the clip is active (played, paused), or 'false' otherwise.
    */
    bool IsClipActive(HashID clipHash);
}

#endif // AUDIO_H_