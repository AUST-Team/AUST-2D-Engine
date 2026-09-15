#pragma once

#ifndef AUDIOSYSTEM_H_
#define AUDIOSYSTEM_H_

#include <unordered_map>
#include <optional>
#include <nlohmann/json.hpp>
#include <list>
#include <filesystem>

#include "AudioBackend.h"
#include "AudioBus.h"
#include "Configuration.h"
#include "AudioClip.h"
#include "FNV1aHash.h"
#include "AudioTrack.h"

struct MIX_Track;

/**
* @brief Singleton for the audio system.
*/
class AudioSystem
{
private:
    AudioBackend backend;   /// Backend for the audio.
    std::unordered_map<HashID, AudioBus> bussesById;    /// Busses (groups) by ID.
    std::unordered_map<HashID, AudioClip> clipsById;    /// Audio clips by ID.
    std::unordered_map<HashID, std::string> bussesIdToName; /// Busses (groups) by name (for debug).
    std::unordered_map<HashID, std::string> clipsIdToName;  /// Audio clips by name (for debug).
    std::list<AudioTrack> activeTracks = {};     /// List of active tracks.
    std::vector<MIX_Track*> finishedTracks = {}; /// Vector of finished tracks. NON OWNING!
    float masterVolume = 1.0f;  /// Master volume. Will be multiplied with a track's own volume. 1.0f = full volume, 0.0f = mute.

    /**
    * @brief Default constructor.
    */
    AudioSystem() = default;

    /**
    * @brief Default destructor.
    */
    ~AudioSystem();

    // No copying or moving allowed due to singleton pattern.
    AudioSystem(const AudioSystem&) = delete;
    AudioSystem& operator=(const AudioSystem&) = delete;
    AudioSystem(AudioSystem&&) = delete;
    AudioSystem& operator=(AudioSystem&&) = delete;

    /**
    * @brief Loads the audio from JSON file.
    * 
    * @param filePath Path to the JSON containing the audio definitions.
    */
    void LoadAudioFromFile(const std::filesystem::path& filePath = Configuration::Get().paths.audioFilePath);

    /**
    * @brief Loads the audio from JSON.
    * 
    * @param audioJSON JSON containing the audio definition.
    */
    void LoadAudioFromJSON(const nlohmann::json& audioJSON);

    void LoadAudioFromFile(const std::string& filePath) = delete;
    void LoadAudioFromFile(const char * filePath) = delete;

public:

    /**
    * @brief Returns the created instance of the audio system.
    * 
    * @return Reference to the created audio system instance.
    */
    static AudioSystem& GetInstance() noexcept
    {
        static AudioSystem instance;
        return instance;
    }

    /**
    * @brief Updates the audio system.
    * 
    * @param deltaTime The delta time of the main app loop.
    */
    void Update(double deltaTime);

    /**
    * @brief Initialises the audio system.
    * 
    * @return 'true' if the audio was sucessfully initialised, 'false' if otherwise.
    */
    bool Init();

    /**
    * @brief Shutdowns the audio system.
    */
    void Shutdown();

    /**
    * @brief Plays a particular clip.
    * 
    * @param clipID Hashed ID of the clip.
    * @param override Optional playback override of the playback.
    */
    void PlayClip(HashID clipID, std::function<void(AudioPlaybackOptions&)> override = nullptr);

    /**
    * @brief Stops a particular clip. Will stop all instances of the clip.
    * 
    * @param clipID Hashed ID of the clip.
    * @param fadeMs Fade out, if any, in milliseconds. 0 for immediate stop.
    */
    void StopClip(HashID clipID, int64_t fadeMs = 0);

    /**
    * @brief Pauses a particular clip. Will pause all instances of the clip.
    *
    * @param clipID Hashed ID of the clip.
    */
    void PauseClip(HashID clipID);

    /**
    * @brief Resumes a particular paused clip. Will resume all instances of the clip, if they're paused.
    *
    * @param clipID Hashed ID of the clip.
    */
    void ResumeClip(HashID clipID);

    /**
    * @brief Plays all tracks with the specified tag.
    * 
    * @param tagID ID of the tag.
    * @param override Optional override for the playback. Will be applied to all tracks. Nullptr for no override.
    */
    void PlayTag(HashID tagID, std::function<void(AudioPlaybackOptions&)> override = nullptr);

    /**
    * @brief Stops all tracks with the specified tag.
    * 
    * @param tagID ID of the tag.
    * @param fadeMs Optional fade out in milliseconds. 0 for no fadeout.
    */
    void StopTag(HashID tagID, int64_t fadeMs = 0);

    /**
    * @brief Pauses all tracks with the specified tag.
    *
    * @param tagID ID of the tag.
    */
    void PauseTag(HashID tagID);

    /**
    * @brief Resumes all tracks with the specified tag.
    *
    * @param tagID ID of the tag.
    */
    void ResumeTag(HashID tagID);

    /**
    * @brief Stops all tracks.
    *
    * @param fadeMs Optional fade out in milliseconds. 0 for no fadeout.
    */
    void StopAll(int64_t fadeMs = 0);

    /**
    * @brief Pauses all tracks.
    */
    void PauseAll();

    /**
    * @brief Resumes all tracks.
    */
    void ResumeAll();

    /**
    * @brief Creates and returns a reference to a bus.
    * 
    * @param tag Tag of the bus (group).
    * 
    * @return Reference to the bus created.
    */
    AudioBus& CreateBus(std::string tag);

    /**
    * @brief Creates a track.
    *
    * @return The track created, or nullptr if track cannot be created.
    */
    AudioTrack* CreateTrack();

    /**
    * @brief Tags a track to a certain bus (tag).
    * 
    * @param track Track to tag.
    * @param bus Tag of the bus.
    */
    void RouteTrack(AudioTrack& track, const std::string& bus);

    /**
    * @brief Marks a track as finished (finished playing and finished all loops).
    * 
    * @param track Track has finished. Tracks that loop infinitely don't finish unless stopped explicitely.
    */
    void MarkTrackFinished(MIX_Track* track);

    /**
    * @brief Returns a bus.
    *
    * @param tagHash Tag hash of the bus.
    *
    * @return Pointer to the bus or nullptr if no bus exists with said tag.
    */
    AudioBus* GetBus(HashID tagHash);

    /**
    * @brief Returns a bus.
    * 
    * @param tag Tag of the bus.
    * 
    * @return Pointer to the bus or nullptr if no bus exists with said tag.
    */
    AudioBus* GetBus(const std::string& tag);

    /**
    * @brief Returns the clips.
    * 
    * @return Reference to the map of clips.
    */
    std::unordered_map<HashID, AudioClip>& GetClips();

    /**
    * @brief Checks if a clip with a given name exists.
    * 
    * @param clipName Name of the clip.
    * 
    * @return 'true' if the clip exists, or 'false' if the clip doesn't exist.
    */
    bool HasClip(const std::string& clipName) const;

    /**
    * @brief Checks if a clip with a given name exists.
    *
    * @param clipHash Hash of the clip.
    * 
    * @return 'true' if the clip exists, or 'false' if the clip doesn't exist.
    */
    bool HasClip(HashID clipHash) const;

    /**
    * @brief Checks if a bus with a given name exists.
    *
    * @param tagName Name of the bus.
    * 
    * @return 'true' if the bus exists, or 'false' if the bus doesn't exist.
    */
    bool HasBus(const std::string& tagName) const;

    /**
    * @brief Checks if a bus with a given name exists.
    *
    * @param tagHash Tag hash of the bus.
    * 
    * @return 'true' if the bus exists, or 'false' if the bus doesn't exist.
    */
    bool HasBus(HashID tagHash) const;

    /**
    * @brief Checks if a clip is currently active, either being played or paused.
    * 
    * @param clipName Name of the clip.
    *
    * @return 'true' if the clip is active (played, paused), or 'false' otherwise.
    */
    bool IsClipActive(const std::string& clipName) const;

    /**
    * @brief Checks if a clip is currently active, either being played or paused.
    *
    * @param clipHash Hash of the clip.
    *
    * @return 'true' if the clip is active (played, paused), or 'false' otherwise.
    */
    bool IsClipActive(HashID clipHash) const;

    /**
    * @brief Returns the master volume.
    * 
    * @return The master volume, between 1.0f and 0.0f.
    */
    float GetMasterVolume() const;

    /**
    * @brief Sets the master volume.
    * 
    * @param newVolume The new master volume, between 1.0f (full volume), and 0.0f (mute).
    */
    void SetMasterVolume(float newVolume);
};

#endif // AUDIOSYSTEM_H_