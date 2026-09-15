#include <SDL3/SDL.h>

#include "GameActionRegistry.h"

#include "TileRegistry.h"
#include "GameMap.h"
#include "TriggerFactory.h"
#include "FlagManager.h"
#include "DialogueSystem.h"
#include "Miscs.h"
#include "Trigger.h"
#include "AudioCommand.h"
#include "Audio.h"
#include "FloatUtils.h"
#include "DialogueBankManager.h"
#include "JSONParser.h"
#include "DataProvider.h"
#include "SystemActionRegistry.h"
#include "PlayerCaughtStrategy.h"
#include "GuardStateManager.h"
#include "GuardCooldownState.h"
#include "MemoryTracker.h"
#include "PlayerStrategyManager.h"

using json = nlohmann::json;

void GameActionRegistry::InitialiseGameActions()
{
    RegisterHandler("setTile", [](const json& params) {

        /*
            What this function expects:

            "params": {
                "position": {"x": 10, "y": 10},
                "tile": "TownSoil",
                "placement": "Replace"
            }
        */

        if (!params.is_object())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameActionRegistry.RegisterHandler: [setTile] Expected object, got non-object.");
            return;
        }

        GameMap::GetInstance().SetTile(params);
    });

    RegisterHandler("setTrigger", [](const json& params) {

        /*
            What this function expects:

            "params": {
                "name": "triggerName",
                "position": {"x": 10, "y": 10},

                "flags": {
                    "repeatable": true,
                    "autoDelete": false
                },

                "conditions": {
                    "type": "interact"
                },

                "interactable": {
                    ...
                }
            }
        */

        if (!params.is_object())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameActionRegistry.RegisterHandler: [setTrigger] Expected object, got non-object.");
            return;
        }

        GameMap::GetInstance().SetTrigger(params, TriggerOrigin::Runtime);
    });

    RegisterHandler("setFlag", [](const json& params) {

        /*
            What this function expects:

            "params": {
                "flag": "flagName",
                "value": true
            }
        */

        if (!params.is_object())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameActionRegistry.RegisterHandler: [setFlag] Expected object, got non-object.");
            return;
        }

        if (!params.contains("flag") || !params["flag"].is_string())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameActionRegistry.RegisterHandler: [setFlag] Parameters do not contain flag name or is not string.");
            return;
        }

        const std::string& flagName = params.at("flag").get_ref<const std::string&>();

        if (flagName.empty())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameActionRegistry.RegisterHandler: [setFlag] Missing flag name.");
            return;
        }

        if (!params.contains("value") || !params["value"].is_boolean())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameActionRegistry.RegisterHandler: [setFlag] Parameters do not contain flag value or is not boolean. Defaulting to false.");
        }

        const bool value = params.value("value", false);

        FlagManager::GetInstance().SetFlag(flagName, value);
    });

    RegisterHandler("setPreset", [](const json& params) {

        /*
            What this function expects:

            "params": {
                "position": {"x": 10, "y": 10},
                "preset": "presetName"
                (optional) "placement": "Add"
            }
        */

        if (!params.is_object())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameActionRegistry.RegisterHandler: [setPreset] Expected object, got non-object.");
            return;
        }

        GameMap::GetInstance().SetPreset(params);
    });

    RegisterHandler("startDialogue", [](const nlohmann::json& params) {

        /*
            What this function expects:

            "params": "nodeName"
            "params": {
                "question": {
                    "bankFile": "questions_easy.json",
                    "count": 1,
                    "randomize": true
                },
                "choices": [
                    {
                        "text": "I lost my way...",
                        "actions": [{ "resumeGame": null }]
                    },
                    {
                        "text": "[Surrender] Take me in.",
                        "actions": [{ "exitGame": null }]
                    }
                ],
                "onEnd": null (or action)
            }
        */

        if (params.is_string())
        {
            const std::string& nodeName = params.get_ref<const std::string&>();

            if (nodeName.empty())
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameActionRegistry.RegisterHandler: [startDialogue] Node name is empty.");
                return;
            }

            DialogueBankManager::ExecuteDialogueData(nodeName);
            return;
        }
        else if (params.is_object())
        {
            const DialogueData data = JSONParser::ParseJSONDialogueData(params);
            DialogueBankManager::ExecuteDialogueData(data);
            return;
        }

        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameActionRegistry.RegisterHandler: [startDialogue] Params is not string or object.");
    });

    RegisterHandler("checkPlayerCaught", [](const nlohmann::json& payload) {

        /*
            What this function expects:

            (full: "checkPlayerCaught": {
                "below": [
                    { "startDialogue": "okNode" },
                    { "playAudio": ... }
                ],
                "above": [
                    { "playCutscene": "badEnding" }
                ]
            })

            payload: {
                "below": [
                    { "startDialogue": "okNode" },
                    { "playAudio": ... }
                ],
                "above": [
                    { "playCutscene": "badEnding" }
                ]
            }
        */

        if (!payload.is_object())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [checkPlayerCaught] Payload is not a JSON object.");
            return;
        }

        const std::optional<int> timesCaught = DataProvider::GetInstance().FetchData<int>(ConstantConfiguration::playerCaughtDataKey);

        if (!timesCaught || *timesCaught < 0)
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [checkPlayerCaught] Key [%s] returned invalid/missing data.", ConstantConfiguration::playerCaughtDataKey);
            return;
        }

        const std::string targetBranch = (*timesCaught < Configuration::Get().player.maxTimesCaught) ? "below" : "above";

        if (!payload.contains(targetBranch) || payload[targetBranch].is_null())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [checkPlayerCaught] Missing branch [%s] in payload.", targetBranch.c_str());
            return;
        }

        const std::vector<SystemAction> actions = JSONParser::ParseJSONSystemActions(payload[targetBranch]);

        for (const SystemAction& action : actions)
        {
            SystemActionRegistry::GetInstance().Execute(action);
        }

    });

    RegisterHandler("changeMap", [](const nlohmann::json& payload) {

        /*
            What this function expects:

            (full: "changeMap": "mapName:")

            payload: "mapName"
        */

        if (!payload.is_string())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [changeMap] Payload is not a JSON string.");
            return;
        }

        const std::string& mapName = payload.get_ref<const std::string&>();

        if (mapName.empty())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [changeMap] Map name is empty.");
            return;
        }

        GameMap::GetInstance().LoadMapFromName(mapName);
    });

    RegisterHandler("setCaughtGuardOnCooldown", [](const nlohmann::json& payload) {

        /*
            What this function expects:

            (full: "setCaughtGuardOnCooldown": <number>)

            payload: <number>
        */

        if (!payload.is_number())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [setCaughtGuardOnCooldown] Payload is not a JSON number.");
            return;
        }

        float timeOut = payload.get<float>();

        if (timeOut < 0.0f)
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [setCaughtGuardOnCooldown] Time out is negative. Defaulting to 5000.0f.");
            timeOut = 5000.0f;
        }

        Guard* guard = PlayerCaughtStrategy::GetInstance().GetGuard();

        if (!guard)
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemActionRegistry.InitialiseSystemActions: [setCaughtGuardOnCooldown] Guard that has caught player is null.");
            return;
        }

        GuardStateManager::SetState(*guard, ENG_NEW(GuardCooldownState, timeOut));
        PlayerStrategyManager::SetStrategy(nullptr);
    });

    RegisterHandler("dialogueNarration", [](const json& params) {

        /*
            What this function expects:

            "params": {
                "lines": [
                    "Line1",
                    "Line2"
                ],
                "onEnd": NULL or "eventName" or specific event -> {
                    "event": "setTile",
                    "params: <setTileParams>
                }
            }
        */

        if (!params.is_object())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameActionRegistry.RegisterHandler: [dialogueNarration] Expected object, got non-object.");
            return;
        }

        if (!params.contains("lines") || !params["lines"].is_array())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameActionRegistry.RegisterHandler: [dialogueNarration] Parameters do not contain dialogue lines or is not an array. Defaulting to empty array.");
        }

        const std::vector<std::string> text = params.value("lines", std::vector<std::string>{});

        if (text.empty()) 
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameActionRegistry.RegisterHandler: [dialogueNarration] Missing text for dialogue narration.");
            return;
        }

        if (params.contains("onEnd") && !params["onEnd"].is_null())
        {
            DialogueSystem::GetInstance().StartDialogue(text, CreateCallbackFromJSON(params["onEnd"]));
        }
        else
        {
            DialogueSystem::GetInstance().StartDialogue(text);
        }

    });

    RegisterHandler("dialogueChoice", [](const json& params) {

        /*
            What this function expects:

            "params": {
                "question": "This is a question",
                "choices": [
                    {
                        "text": "Choice 1",
                        "callback": "event1"
                    },
                    {
                        "text": "Second choice",
                        "callback": "event2"
                    },
                    {
                        "text": "Number 3",
                        "callback": "event3"
                    },
                    {
                        "text": "4",
                        "callback": "event4"
                    }
                ]
            }
        */

        if (!params.is_object())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameActionRegistry.RegisterHandler: [dialogueChoice] Expected object, got non-object.");
            return;
        }

        if (!params.contains("question") || !params["question"].is_string())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameActionRegistry.RegisterHandler: [dialogueChoice] Missing question for dialogue choice. Defaulting to empty string");
        }

        const std::string question = params.value("question", "");

        if (!params.contains("choices") || !params["choices"].is_array()) 
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameActionRegistry.RegisterHandler: [dialogueChoice] Choices for dialogue choice are missing or are not in an array.");
            return;
        }

        std::vector<std::string> choices;
        std::vector<std::function<void()>> callbacks;

        for (const json& choice : params["choices"]) 
        {
            std::string choiceText = choice.value("text", "");
            choices.push_back(choiceText);

            if (choice.contains("callback") && !choice["callback"].is_null())
            {
                callbacks.push_back(CreateCallbackFromJSON(choice["callback"]));
            }
            else
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameActionRegistry.RegisterHandler: [dialogueChoice] Choice callback for dialogue choice is missing or is null. Defaulting to no-op.");
                callbacks.push_back([]() {});
            }
        }

        DialogueSystem::GetInstance().StartChoice(choices, callbacks, question);
    });
}

void GameActionRegistry::RegisterHandler(const std::string& name, std::function<void(const json&)> callback) { handlers[name] = callback; }

void GameActionRegistry::Execute(const std::string& name, const json& params) const
{
    auto it = handlers.find(name);
    if (it != handlers.end())
    {
        it->second(params);
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameActionRegistry.Execute: No event found with name '%s'", name.c_str());
    }
}