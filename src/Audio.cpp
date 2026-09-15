#include <SDL3/SDL_log.h>

#include "Audio.h"

#include "AudioSystem.h"
#include "AudioTrack.h"
#include "AudioCommand.h"

void Audio::Play(const std::string& clipName, std::function<void(AudioPlaybackOptions&)> override)
{
    Play(FNV1aHash::HashString(clipName), override);
}

void Audio::Play(const std::string& clipName, const AudioPlaybackOptions & override)
{
    Play(FNV1aHash::HashString(clipName), [override](AudioPlaybackOptions& opts) {
        opts = override;
    });
}

void Audio::Stop(const std::string& clipName, int64_t fadeMs)
{
    Stop(FNV1aHash::HashString(clipName), fadeMs);
}

void Audio::Pause(const std::string& clipName)
{
    Pause(FNV1aHash::HashString(clipName));
}

void Audio::Resume(const std::string& clipName)
{
    Resume(FNV1aHash::HashString(clipName));
}

void Audio::PlayTag(const std::string& tag, std::function<void(AudioPlaybackOptions&)> override)
{
    PlayTag(FNV1aHash::HashString(tag), override);
}

void Audio::PlayTag(const std::string& tag, const AudioPlaybackOptions& override)
{
    PlayTag(FNV1aHash::HashString(tag), [override](AudioPlaybackOptions& opts) {
        opts = override;
    });
}

void Audio::StopTag(const std::string& tag, int64_t fadeMs)
{
    StopTag(FNV1aHash::HashString(tag), fadeMs);
}

void Audio::PauseTag(const std::string& tag)
{
    PauseTag(FNV1aHash::HashString(tag));
}

void Audio::ResumeTag(const std::string& tag)
{
    ResumeTag(FNV1aHash::HashString(tag));
}

void Audio::StopAll(int64_t fadeMs)
{
    AudioSystem::GetInstance().StopAll(fadeMs);
}

void Audio::PauseAll()
{
    AudioSystem::GetInstance().PauseAll();
}

void Audio::ResumeAll()
{
    AudioSystem::GetInstance().ResumeAll();
}

void Audio::Play(HashID clipID, std::function<void(AudioPlaybackOptions&)> override)
{
    AudioSystem::GetInstance().PlayClip(clipID, override);
}

void Audio::Play(HashID clipID, const AudioPlaybackOptions& override)
{
    Play(clipID, [override](AudioPlaybackOptions& opts) {
        opts = override;
    });
}

void Audio::Stop(HashID clipID, int64_t fadeMs)
{
    AudioSystem::GetInstance().StopClip(clipID, fadeMs);
}

void Audio::Pause(HashID clipID)
{
    AudioSystem::GetInstance().PauseClip(clipID);
}

void Audio::Resume(HashID clipID)
{
    AudioSystem::GetInstance().ResumeClip(clipID);
}

void Audio::PlayTag(HashID tagID, std::function<void(AudioPlaybackOptions&)> override)
{
    AudioSystem::GetInstance().PlayTag(tagID, override);
}

void Audio::PlayTag(HashID tagID, const AudioPlaybackOptions& override)
{
    PlayTag(tagID, [override](AudioPlaybackOptions& opts) {
        opts = override;
    });
}

void Audio::StopTag(HashID tagID, int64_t fadeMs)
{
    AudioSystem::GetInstance().StopTag(tagID, fadeMs);
}

void Audio::PauseTag(HashID tagID)
{
    AudioSystem::GetInstance().PauseTag(tagID);
}

void Audio::ResumeTag(HashID tagID)
{
    AudioSystem::GetInstance().ResumeTag(tagID);
}

void Audio::ExecuteAudioCommand(const AudioCommand& command)
{
    switch (command.targetType)
    {
        case AudioTargetType::Clip:
        {
            ExecuteClipAudioCommand(command);
            break;
        }

        case AudioTargetType::Tag:
        {
            ExecuteTagAudioCommand(command);
            break;
        }

        case AudioTargetType::All:
        {
            ExecuteGlobalAudioCommand(command);
            break;
        }

        default:
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Audio.ExecuteAudioCommand: Invalid audio target type.");
            break;
		}
    }
}

void Audio::ExecuteClipAudioCommand(const AudioCommand& command)
{
    switch (command.action)
    {
        case AudioAction::Play:
        {
            Play(command.targetID, command.opts.playbackOverride);
            break;
        }

        case AudioAction::Stop:
        {
            Stop(command.targetID, command.opts.fadeMs);
            break;
        }

        case AudioAction::Pause:
        {
            Pause(command.targetID);
            break;
        }

        case AudioAction::Resume:
        {
            Resume(command.targetID);
            break;
        }

        case AudioAction::PlayOrResume:
        {
            if (IsClipActive(command.targetID))
            {
                Resume(command.targetID);
            }
            else
            {
                Play(command.targetID, command.opts.playbackOverride);
            }
            break;
        }

        default:         
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Audio.ExecuteClipAudioCommand: Invalid audio action.");
            break;
		}
    }
}

void Audio::ExecuteTagAudioCommand(const AudioCommand& command)
{
    switch (command.action)
    {
        case AudioAction::Play:
        {
            PlayTag(command.targetID, command.opts.playbackOverride);
            break;
        }

        case AudioAction::Stop:
        {
            StopTag(command.targetID, command.opts.fadeMs);
            break;
        }

        case AudioAction::Pause:
        {
            PauseTag(command.targetID);
            break;
        }

        case AudioAction::Resume:
        {
            ResumeTag(command.targetID);
            break;
        }

        default:
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Audio.ExecuteTagAudioCommand: Invalid audio action.");
            break;
        }
    }
}

void Audio::ExecuteGlobalAudioCommand(const AudioCommand& command)
{
    switch (command.action)
    {
        case AudioAction::Stop:
        {
            StopAll(command.opts.fadeMs);
            break;
        }

        case AudioAction::Pause:
        {
            PauseAll();
            break;
        }

        case AudioAction::Resume:
        {
            ResumeAll();
            break;
        }

        case AudioAction::Play:
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Audio.ExecuteGlobalAudioCommand: Attempted to execute 'play' action on global target.");
            break;
        }

        default:
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Audio.ExecuteGlobalAudioCommand: Invalid audio action.");
            break;
		}
    }
}

float Audio::GetMasterVolume() { return AudioSystem::GetInstance().GetMasterVolume(); }

void Audio::SetMasterVolume(float newVolume) { AudioSystem::GetInstance().SetMasterVolume(newVolume); }

bool Audio::IsClipActive(const std::string& clipName) { return IsClipActive(FNV1aHash::HashString(clipName)); }

bool Audio::IsClipActive(HashID clipHash) { return AudioSystem::GetInstance().IsClipActive(clipHash); }
