#include <SDL3/SDL_log.h>

#include "SystemActionRegistry.h"
#include "GameActionRegistry.h"

#include "Audio.h"
#include "StringMiscs.h"
#include "GameTime.h"
#include "AppStateManager.h"
#include "MemoryTracker.h"
#include "MainMenuState.h"
#include "GameState.h"
#include "AppRenderSystem.h"
#include "CutsceneState.h"
#include "DataProvider.h"
#include "TogglePauseCommand.h"
#include "ScoreManager.h"
#include "DialogueBankManager.h"
#include "SystemValueRegistry.h"
#include "JSONParser.h"
#include "Unused.h"

void SystemActionRegistry::RegisterHandler(const std::string& actionName, const ActionHandler& handler) { handlers[actionName] = handler; }

void SystemActionRegistry::Execute(const std::string& actionName, const nlohmann::json& payload)
{
    if (actionName.rfind("game:", 0) == 0)
    {
        std::string eventName = actionName.substr(5);
        GameActionRegistry::GetInstance().Execute(eventName, payload);
        return;
    }

    auto it = handlers.find(actionName);
    if (it != handlers.end()) 
    {
        it->second(payload);
    }
    else 
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.Execute: Unhandled action: %s", actionName.c_str());
    }
}

void SystemActionRegistry::Execute(const SystemAction& action) { Execute(action.type, action.payload); }

void SystemActionRegistry::InitialiseSystemActions()
{
    RegisterHandler("quitToMainMenu", [](const nlohmann::json& payload) {

        /*
			What this function expects:

			(full: "quitToMainMenu": null)

			playload: null
        */

        UNUSED(payload);

        GameTime::StopTracking();
        Audio::StopAll();
        AppStateManager::GetInstance().ScheduleStateChange(ENG_NEW(MainMenuState));
    });

    RegisterHandler("exitGame", [](const nlohmann::json& payload) {

        /*
            What this function expects:

            (full: "exitGame": null)

            playload: null
        */

        UNUSED(payload);

        AppStateManager::GetInstance().ScheduleQuit();
    });

    RegisterHandler("resumeGame", [](const nlohmann::json& payload) {
        /*
            What this function expects:

            (full: "resumeGame": null)

            playload: null
        */

        UNUSED(payload);

        TogglePauseCommand cmd;
		cmd.Execute();
    });

    RegisterHandler("startGame", [](const nlohmann::json& payload) {
        /*
            What this function expects:

            (full: "startGame": null)

            playload: null
        */

        UNUSED(payload);

        AppStateManager::GetInstance().ScheduleStateChange(ENG_NEW(GameState));
    });

    RegisterHandler("toggleFullscreen", [](const nlohmann::json& payload) {

        /*
            What this function expects:

			(full: "toggleFullscreen": true / false (injected from checkbox) })

            playload: true / false
        */

        bool isFullscreen = !AppRenderSystem::GetInstance().IsFullscreen();

        if (!payload.is_boolean())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [toggleFullscreen] payload is not bool. Trying to use app fullscreen status.");
        }
        else
        {
            isFullscreen = payload.get<bool>();
        }

        AppRenderSystem::GetInstance().ScheduleFullScreen(isFullscreen);
    });

    RegisterHandler("audioControl", [](const nlohmann::json& payload) {

        /*
            What this function expects:

            Note that payload is the default JSON object.

            {
                "target": {
                    "type": "clip",
                    "id": "map1Music"
                },
                "action": "play"
                "playback": {
                    "loop": true,
                    "loopCount": 3,
                    "fadeIn": 1000,
                    "volume": 0.5
                }
            }

            {
                "target": {
                    "type": "tag",
                    "id": "SFX"
                },
                "action": "stop",
                "fadeOut": 500
            }

            {
                "target": {
                    "type": "all"
                },
                "action": "pause"
            }
        */

        const AudioCommand command = JSONParser::ParseJSONAudioCommand(payload);
        Audio::ExecuteAudioCommand(command);
    });

    // A less sophisticated version of audioControl.
    RegisterHandler("playAudio", [](const nlohmann::json& payload) {
        
        /*
            What this function expects:

            "payload": "button_click"
            "payload": { 
                "name": "button_click", 
                "options": { 
                    "volume": 0.8, 
                    "loop": true,
                    "loopCount": 3,
                    "fadeIn": 1
                } 
            }
        */

        std::string audioKey;
        nlohmann::json options = nullptr;

        if (payload.is_string())
        {
            audioKey = payload.get<std::string>();
        }
        else if (payload.is_object())
        {

            if (!payload.contains("name") || !payload["name"].is_string())
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [playAudio] Payload is object, but missing \"name\" key.");
                return;
            }

            audioKey = payload.at("name").get<std::string>();
            if (payload.contains("options") )
            {
                if (payload["options"].is_object())
                {
                    options = payload["options"];
                }
                else if(!payload["options"].is_null())
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [playAudio] Payload is object has \"options\" key, but is not object or null.");
                }
            }
            else
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [playAudio] Payload is object but has no \"options\" key. If you don't wish for options, please put the key with a null value.");
            }
        }

        if (audioKey.empty())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [playAudio] Missing audio key.");
            return;
        }

        if (!options.is_null())
        {
            AudioPlaybackOptions audioOpts = JSONParser::ParseJSONAudioPlaybackOptions(options);
            Audio::Play(audioKey, audioOpts);
        }
        else
        {
            Audio::Play(audioKey);
        }
    });

    RegisterHandler("pauseAudio", [](const nlohmann::json& payload) {

        /*
            What this function expects:

            "payload": "<clipName>" or NULL for all audio
        */

        if (payload.is_string())
        {
            Audio::Pause(payload.get_ref<const std::string&>());
            return;
        }
        else if (payload.is_null())
        {
            Audio::PauseAll();
            return;
        }

        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [pauseAudio] Payload is not string or null.");
    });

    RegisterHandler("stopAudio", [](const nlohmann::json& payload) {

        /*
            What this function expects:

            "payload": "<clipName>" or NULL for all audio

            "payload": {
                "name": "<clipName>" or NULL
                "fadeOut": <number>
            }
        */

        if (payload.is_string())
        {
            Audio::Stop(payload.get_ref<const std::string&>());
            return;
        }
        else if (payload.is_object())
        {
            std::string audioKey;
            if (payload.contains("name"))
            {
                if (payload["name"].is_string())
                {
                    audioKey = payload.at("name").get<std::string>();
                }
                else if (!payload["name"].is_null())
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [stopAudio] Payload is object and has \"name\" key, but is not string or null. Defaulting to stopping all audio.");
                }
            }
            else
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [stopAudio] Payload is object, but has no \"name\" key.");
                return;
            }

            int64_t fadeOut = 0;
            if (payload.contains("fadeOut"))
            {
                if (payload["fadeOut"].is_object() || payload["fadeOut"].is_number())
                {
                    fadeOut = JSONParser::ParseJSONTime<int64_t>(payload["fadeOut"], fadeOut);
                }
                else if (!payload["fadeOut"].is_null())
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [stopAudio] Payload is object and has \"fadeOut\" key, but is not object, number or null.");
                }
            }
            else
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [stopAudio] Payload is object, but has no \"fadeOut\" key. If no fadeout is wanted, please put the key with a null value to supress this.");
            }

            if (fadeOut < 0)
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [stopAudio] Fade out is negative, defaulting to '0' (no fadeout).");
                fadeOut = 0;

            }

            if (audioKey.empty())
            {
                Audio::StopAll(fadeOut);
            }
            else
            {
                Audio::Stop(audioKey, fadeOut);
            }

            return;
        }
        else if (payload.is_null())
        {
            Audio::StopAll();
            return;
        }

        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [stopAudio] Payload is not object, string or null.");
    });

    RegisterHandler("resumeAudio", [](const nlohmann::json& payload) {

        /*
            What this function expects:

            "payload": "<clipName>"
        */

        if (payload.is_string())
        {
            Audio::Resume(payload.get_ref<const std::string&>());
            return;
        }

        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [resumeAudio] Payload is not string.");
    });

    RegisterHandler("playOrResumeAudio", [](const nlohmann::json& payload) {

        /*
            What this function expects:

            "payload": "<clipName>"
            "payload": {
                "name": "<clipName>",
                "options": {
                    "volume": 0.8,
                    "loop": true,
                    "loopCount": 3,
                    "fadeIn": 1
                }
            }
        */

        std::string audioKey;

        if (payload.is_string())
        {
            audioKey = payload.get<std::string>();
        }
        else if (payload.is_object())
        {

            if (!payload.contains("name") || !payload["name"].is_string())
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [playOrResumeAudio] Payload is object, but missing \"name\" key.");
                return;
            }

            audioKey = payload.at("name").get<std::string>();
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [playOrResumeAudio] Payload is not string or object.");
        }

        if (audioKey.empty())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [playOrResumeAudio] Missing audio key.");
            return;
        }

        if (Audio::IsClipActive(audioKey))
        {
            SystemActionRegistry::GetInstance().Execute("resumeAudio", payload);
            return;
        }
        else
        {
            SystemActionRegistry::GetInstance().Execute("playAudio", payload);
            return;
        }
    });

    RegisterHandler("playCutscene", [](const nlohmann::json& payload) {

        /*
			What this function expects:
            
            (full: "playCutscene": "cutsceneName")

			payload: "cutsceneName"
        */

        if (!payload.is_string())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [playCutscene] Payload is not string.");
            return;

        }

        const std::string& cutsceneName = payload.get_ref<const std::string&>();

        if (cutsceneName.empty())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [playCutscene] Cutscene name is missing or empty.");
            return;
        }

        AppStateManager::GetInstance().ScheduleStateChange(ENG_NEW(CutsceneState, cutsceneName));
    });

    RegisterHandler("submitScore", [](const nlohmann::json& payload) {

        /*
            What this function expects:

            (full: "submitScore": "playerNameInputKey")

			payload: "playerNameInputKey"
        */

        if (!payload.is_string())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [submitScore] Payload is not string.");
            return;
        }

        const std::string& dataKey = payload.get_ref<const std::string&>();

        if (dataKey.empty())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [submitScore] Data key for player name is empty.");
            return;
        }

        const std::optional<std::string> nameOpt = DataProvider::GetInstance().FetchData<std::string>(dataKey);
        if (!nameOpt || nameOpt->empty())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [submitScore] Failed to retrieve player name for key '%s'.", dataKey.c_str());
            return;
        }

        const std::optional<int> caughtOpt = DataProvider::GetInstance().FetchData<int>(ConstantConfiguration::playerCaughtDataKey);
        if (!caughtOpt || *caughtOpt < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [submitScore] Invalid or missing player caught count.");
            return;
        }

        const uint64_t playTime = GameTime::GetPlayTimeSeconds();
        ScoreManager scoreManager;
        scoreManager.LoadScoresFromFile();
        scoreManager.AddScore(*nameOpt, *caughtOpt, playTime);
    });

    RegisterHandler("setMasterVolume", [](const nlohmann::json& payload) {

        /*
            What this function expects:

            payload: <number between 0 - 100>
        */

        if (!payload.is_number())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [setMasterVolume] Payload is not number.");
            return;
        }

        int volume = payload.get<int>();

        if (volume < 0)
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [setMasterVolume] Volume is negative. Setting to 0.");
        }
        else if (volume > 100)
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [setMasterVolume] Volume is above 100. Setting to 100.");
        }

        volume = std::clamp(volume, 0, 100);

        float volumeNorm = volume / 100.0f;

        Audio::SetMasterVolume(volumeNorm);
    });
}
