#include <SDL3/SDL_log.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <fstream>
#include <nlohmann/json.hpp>

#include "AudioSystem.h"
#include "AudioClip.h"
#include "AudioTrack.h"
#include "Audio.h"
#include "Configuration.h"
#include "FloatUtils.h"
#include "FileLoader.h"
#include "MemoryTracker.h"
#include "JSONParser.h"
#include "Unused.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

AudioSystem::~AudioSystem() { Shutdown(); }

void AudioSystem::Update(double deltaTime)
{
    UNUSED(deltaTime);
    std::erase_if(activeTracks, [this](const AudioTrack& t) {
        return std::find(finishedTracks.begin(), finishedTracks.end(), t.GetTrack()) != finishedTracks.end();
    });

    finishedTracks.clear();
}

bool AudioSystem::Init()
{
    if (!backend.Init())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.Init: Failed to initialise backend.");
        return false;
    }

    CreateBus("default");
    LoadAudioFromFile();

    return true;
}

void AudioSystem::Shutdown()
{
    bussesById.clear();
    bussesIdToName.clear();
    clipsById.clear();
    clipsIdToName.clear();
    backend.Shutdown();
}

void AudioSystem::PlayClip(HashID clipID, std::function<void(AudioPlaybackOptions&)> override)
{
    auto it = clipsById.find(clipID);
    if (it == clipsById.end())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.PlayClip: Unknown clip");
        return;
    }

    const AudioClip& clip = it->second;

    AudioTrack* track = CreateTrack();

    if (!track)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.PlayClip: Create track retuned null track.");
        return;
    }

    track->SetAudio(clip);

    if (const AudioBus* bus = GetBus(clip.busID))
    {
        bus->TagTrack(*track);
    }

    AudioPlaybackOptions opts = clip.defaultOptions;    // COPY!
    opts.SetVolume(opts.GetVolume());

    if (override)
    {
        override(opts);
    }

    track->Play(&opts);
}

void AudioSystem::StopClip(HashID clipID, int64_t fadeMs)
{
    for (const AudioTrack& track : activeTracks)
    {
        if (track.GetAudioID() == clipID)
        {
            int64_t fadeFrames = MIX_TrackMSToFrames(track.GetTrack(), fadeMs);

            if (fadeFrames == -1)
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.StopClip: Failed TrackMSToFrames: %s. Defaulting to no fade out.", SDL_GetError());
                fadeFrames = 0;
            }

            if (!MIX_StopTrack(track.GetTrack(), fadeFrames))
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.StopClip: Failed StopTrack: %s.", SDL_GetError());
            }
        }
    }
}

void AudioSystem::PauseClip(HashID clipID)
{
    for (const AudioTrack& track : activeTracks)
    {
        if (track.GetAudioID() == clipID)
        {
            if (!MIX_PauseTrack(track.GetTrack()))
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.PauseClip: Failed PauseTrack: %s.", SDL_GetError());
            }
        }
    }
}

void AudioSystem::ResumeClip(HashID clipID)
{
    for (const AudioTrack& track : activeTracks)
    {
        if (track.GetAudioID() == clipID)
        {
            if (!MIX_ResumeTrack(track.GetTrack()))
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.PauseClip: Failed PauseTrack: %s.", SDL_GetError());
            }
        }
    }
}

void AudioSystem::PlayTag(HashID tagID, std::function<void(AudioPlaybackOptions&)> override)
{
    for (auto& [id, clip] : clipsById)
    {
        if (clip.busID == tagID)
        {
            PlayClip(id, override);
        }
    }
}

void AudioSystem::StopTag(HashID tagID, int64_t fadeMs)
{
    auto it = bussesById.find(tagID);
    if (it == bussesById.end())
    {
        return;
    }

    it->second.StopAll(fadeMs);
}

void AudioSystem::PauseTag(HashID tagID)
{
    auto it = bussesById.find(tagID);
    if (it == bussesById.end())
    {
        return;
    }

    it->second.PauseAll();
}

void AudioSystem::ResumeTag(HashID tagID)
{
    auto it = bussesById.find(tagID);
    if (it == bussesById.end())
    {
        return;
    }

    it->second.ResumeAll();
}

void AudioSystem::StopAll(int64_t fadeMs)
{
    if (!MIX_StopAllTracks(backend.GetMixer(), fadeMs))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.StopAll: Failed StopAllTracks: %s", SDL_GetError());
    }
}

void AudioSystem::PauseAll()
{
    if (!MIX_PauseAllTracks(backend.GetMixer()))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.PauseAll: Failed PauseAllTracks: %s", SDL_GetError());
    }
}

void AudioSystem::ResumeAll()
{
    if (!MIX_ResumeAllTracks(backend.GetMixer()))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.ResumeAll: Failed ResumeAllTracks: %s", SDL_GetError());
    }
}

AudioBus& AudioSystem::CreateBus(std::string tag)
{
    const HashID tagHash = FNV1aHash::HashString(tag);

    // Check if bus already exists.
    auto it = bussesById.find(tagHash);
    if (it != bussesById.end())
    {
        const std::string& busTag = bussesIdToName.at(tagHash);
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.CreateBus: Bus [%s] already exists or hash collision with [%s].", tag.c_str(), busTag.c_str());
        return it->second;
    }

    bussesIdToName.emplace(
        tagHash,
        tag
    );

    auto [bus, inserted] = bussesById.emplace(
        tagHash,
        AudioBus(tag, backend.GetMixer()) 
    );

    return bus->second;
}

AudioBus* AudioSystem::GetBus(HashID tagHash)
{
    auto it = bussesById.find(tagHash);
    return it != bussesById.end() ? &it->second : nullptr;
}

AudioBus* AudioSystem::GetBus(const std::string& tag) { return GetBus(FNV1aHash::HashString(tag)); }

std::unordered_map<HashID, AudioClip>& AudioSystem::GetClips() { return clipsById; }

bool AudioSystem::HasClip(const std::string& clipName) const { return HasClip(FNV1aHash::HashString(clipName)); }

bool AudioSystem::HasClip(HashID clipHash) const { return clipsById.contains(clipHash); }

bool AudioSystem::HasBus(const std::string& tagName) const { return HasBus(FNV1aHash::HashString(tagName)); }

bool AudioSystem::HasBus(HashID tagHash) const { return bussesById.contains(tagHash); }

bool AudioSystem::IsClipActive(const std::string& clipName) const { return IsClipActive(FNV1aHash::HashString(clipName)); }

bool AudioSystem::IsClipActive(HashID clipHash) const
{
    for (const AudioTrack& track : activeTracks)
    {
        if (track.GetAudioID() == clipHash)
        {
            return true;
        }
    }
    return false;
}

float AudioSystem::GetMasterVolume() const { return masterVolume; }

void AudioSystem::SetMasterVolume(float newVolume) 
{
    masterVolume = std::clamp(newVolume, 0.0f, 1.0f); 
    MIX_SetMixerGain(backend.GetMixer(), masterVolume);
}

AudioTrack* AudioSystem::CreateTrack()
{
    MIX_Track* rawTrack = ENG_MIX_CreateTrack(backend.GetMixer());
    if (!rawTrack)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.CreateTrack: Failed to create track: %s", SDL_GetError());
        return nullptr;
    }

    activeTracks.emplace_back(rawTrack);

    AudioTrack& track = activeTracks.back();

    const bool success = MIX_SetTrackStoppedCallback(
        track.GetTrack(),
        &TrackFinishedCallback,
        this
    );

    if (!success)
    {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.CreateTrack: Failed to set track stopped callback: %s", SDL_GetError());
        ENG_MIX_DestroyTrack(rawTrack);
        activeTracks.pop_back();
        return nullptr;
    }

    return &track;
}

void AudioSystem::RouteTrack(AudioTrack& track, const std::string& bus)
{
    if (const AudioBus* b = GetBus(bus))
    {
        b->TagTrack(track);
    }
}

void AudioSystem::MarkTrackFinished(MIX_Track* track) { finishedTracks.push_back(track); }

void AudioSystem::LoadAudioFromFile(const fs::path& filePath)
{
    /*
        What this function expects:

        A audio.json file.
    */

    const std::optional<json>& jsonOptional = FileLoader::LoadJSON(filePath);

    if (!jsonOptional)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.LoadAudioFromFile: Failed to load JSON at: %s.", filePath.string().c_str());
        return;
    }

    LoadAudioFromJSON(*jsonOptional);
}

void AudioSystem::LoadAudioFromJSON(const json& audioJSON)
{
    if (!audioJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.LoadAudioFromJSON: Expected object, got non-object.");
        return;
    }

    /*
        What this function expects:

        Note that 'audioJSON' is the default JSON object.

        {
            "busses": [
                "Master",
                "Music",
                "SFX",
                "UI"
            ],

            "sounds": [
                {
                    "name": "ui_click",
                    "bus": "UI",
                    "file": "ui/click.mp3",
                    "playback": {
                        "volume": 0.1
                    }
                },
                {
                    "name": "map1music",
                    "bus": "Music",
                    "file": "music/map1.mp3",
                    "playback": {
                        "volume": 1.0,
                        "loop": true,
                        "fadeInMs": 0,
                        "loopCount": -1
                    }
                }
            ]
        }
    */

    if (!audioJSON.contains("busses") || !audioJSON["busses"].is_array())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.LoadAudioFromJSON: Audio JSON doesn't contain busses or is not array.");
        return;
    }

    for (const json& bus : audioJSON["busses"])
    {
        CreateBus(bus.get<std::string>());
    }

    for (const json& s : audioJSON["sounds"])
    {
        if (!s.is_object())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.LoadAudioFromJSON: Audio entry is not object. Skipping.");
            continue;
        }

        if(!s.contains("name") || !s["name"].is_string())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.LoadAudioFromJSON: Audio entry doesn't contain name or is not string. Skipping.");
            continue;
		}

        std::string name = s.at("name").get<std::string>();

        if (!s.contains("file") || !s["file"].is_string())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.LoadAudioFromJSON: Audio entry [%s] doesn't contain file path to the audio or is not string. Skipping.", name.c_str());
            continue;
        }

        if (!s.contains("bus") || !s["bus"].is_string())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.LoadAudioFromJSON: Audio entry [%s] doesn't contain bus or is not string. Defaulting to \"default\".", name.c_str());
        }

        if (!s.contains("playback") || !s["playback"].is_object())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.LoadAudioFromJSON: Audio entry [%s] doesn't contain playback options or is not object. This could be intentional.", name.c_str());
        }

        std::string bus = s.value("bus", "default");

        AudioAsset audio = LoadAudio(Configuration::Get().paths.audioDirectory / JSONParser::ParseJSONPath(s["file"]), backend.GetMixer());

        if (!audio.GetAudio())
        {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.LoadAudioFromJSON: Audio entry [%s] failed to load audio. Skipping.", name.c_str());
            return;
        }

        AudioClip clip {
            .clipID = FNV1aHash::HashString(name),
            .busID = FNV1aHash::HashString(bus),
            .asset = std::move(audio),
            .defaultOptions = JSONParser::ParseJSONAudioPlaybackOptions(s.value("playback", json::object()))
        };

        if (clipsById.contains(clip.clipID))
        {
            const std::string& it = clipsIdToName.at(clip.clipID);
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioSystem.LoadAudioFromJSON: Audio entry with name [%s] already exists or hash collission with [%s].", name.c_str(), it.c_str());
            return;
        }

        const HashID clipID = clip.clipID;

        clipsById.emplace(
            clipID,
            std::move(clip)
        );

        clipsIdToName.emplace(
            clipID,
            std::move(name)
        );
    }
}

