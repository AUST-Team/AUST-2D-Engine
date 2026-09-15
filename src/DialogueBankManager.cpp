#include <SDL3/SDL.h>
#include <fstream>
#include <random>

#include "DialogueBankManager.h"

#include "JSONParser.h"
#include "SystemActionRegistry.h"
#include "DialogueSystem.h"
#include "DialogueData.h"
#include "Configuration.h"
#include "FileLoader.h"
#include "IOMiscs.h"
#include "Player.h"
#include "PlayerStrategy.h"
#include "PlayerStrategyManager.h"
#include "PlayerDialogueStrategy.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

void DialogueBankManager::ExecuteDialogueData(const DialogueData& data)
{
    /*
        What this function expects:

        Note that dataJSON is the default JSON object.

        {
            "question": { "bankFile": "questions_easy.json", "count": 1, "randomize": true },
            "choices": [
                {
                    "text": "[Try to bribe the guard]",
                    "actions": [{ "startDialogue": "goodJob" }]
                },
                {
                    "text": "[Try to be nice]",
                    "actions": [{ "exitGame": null }]
                }
            ]
        }

        {
            "lines": {
                "bankFile": "NPC1_dialogue"
            }
        }

        "value": "nodeName"

        {
            "node": "nodeName"
        }

    */

    const json& dataJSON = data.data;

    if (!dataJSON.is_object() && !dataJSON.is_string())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueBankManager.ExecuteDialogueData: Dialogue data JSON is not object or string.");
        return;
    }

    std::vector<SystemAction> onEndActions;
    if (dataJSON.contains("onEnd") && !dataJSON["onEnd"].is_null())
    {
        onEndActions = JSONParser::ParseJSONSystemActions(dataJSON["onEnd"]);
    }

    std::function<void()> triggerOnEnd = nullptr;
    if (!onEndActions.empty())
    {
         triggerOnEnd = [actions = std::move(onEndActions)]() {
             for (const SystemAction& action : actions)
             {
                 SystemActionRegistry::GetInstance().Execute(action);
             }
        };
    }

    switch (data.type)
    {
        case DialogueType::Narration:
        {
            std::vector<std::string> lines = DialogueBankManager::GetLines(dataJSON["lines"]);

            if (PlayerStrategyManager::GetCurrentStrategyType() != PlayerStrategyType::Dialogue)
            {
                PlayerStrategyManager::SetStrategy(&PlayerDialogueStrategy::GetInstance());
            }

            DialogueSystem::GetInstance().StartDialogue(lines, triggerOnEnd);
            return;
        }
        case DialogueType::Choice:
        {
            std::vector<std::string> questionLines = DialogueBankManager::GetLines(dataJSON["question"]);
            std::string questionText = questionLines.empty() ? "" : questionLines.front();

            const bool randomizeAllChoices = dataJSON.value("randomizeAllChoices", false);

            // Extract and expand choices (handles dynamic bank expansions).
            std::vector<ResolvedChoice> resolvedChoices = ParseAndExpandChoices(dataJSON["choices"], randomizeAllChoices);

            std::vector<std::string> choiceTexts;
            std::vector<std::function<void()>> choiceCallbacks;
            choiceTexts.reserve(resolvedChoices.size());
            choiceCallbacks.reserve(resolvedChoices.size());

            for (const ResolvedChoice& choice : resolvedChoices)
            {
                choiceTexts.push_back(choice.text);
                choiceCallbacks.push_back([actions = choice.actions]() {
                    for (const SystemAction& action : actions)
                    {
                        SystemActionRegistry::GetInstance().Execute(action);
                    }
                });
            }

            if (PlayerStrategyManager::GetCurrentStrategyType() == PlayerStrategyType::Normal)
            {
                PlayerStrategyManager::SetStrategy(&PlayerDialogueStrategy::GetInstance());
            }

            DialogueSystem::GetInstance().StartChoice(choiceTexts, choiceCallbacks, questionText, triggerOnEnd);
            return;
        }
        case DialogueType::Node:
        {
            std::string nodeName;

            if (data.data.is_object() && data.data.contains("node"))
            {
                nodeName = data.data.value("node", "");
            }
            else if (data.data.is_string())
            {
                nodeName = data.data.get<std::string>();
            }

            if (nodeName.empty())
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueBankManager.ExecuteDialogueData: Empty node path.");
                return;
            }

            DialogueBankManager::ExecuteDialogueData(nodeName);
            return;
        }

        default:
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueBankManager.ExecuteDialogueData: Unknown type of dialogue [%u]", data.type);
            return;
        }
    }
}

void DialogueBankManager::ExecuteDialogueData(const nlohmann::json& dataJSON)
{
    const DialogueData data = JSONParser::ParseJSONDialogueData(dataJSON);
    ExecuteDialogueData(data);
}

void DialogueBankManager::ExecuteDialogueData(const std::string& nodeName)
{
    const GameConfiguration& config = Configuration::Get();
    const fs::path filePath = config.paths.dialogueDirectory / IOMiscs::MakeCrossPlatformPath(nodeName + ".json");

    if (!fs::exists(filePath))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueBankManager.ExecuteDialogueData: Dialogue node [%s] path does not exist: %s", nodeName.c_str(), filePath.string().c_str());
        return;
    }

    std::optional<json> jsonOpt = FileLoader::LoadJSON(filePath);

    if (!jsonOpt)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueBankManager.ExecuteDialogueData: Failed to load file for node [%s] at %s", nodeName.c_str(), filePath.string().c_str());
        return;
    }

    ExecuteDialogueData(*jsonOpt);
}

std::vector<std::string> DialogueBankManager::LoadDialogueFromFile(const std::filesystem::path& filePath, int count, bool randomize)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "DialogueBankManager.LoadDialogueFromFile: Failed to open: %s", filePath.string().c_str());
        return {};
    }

    nlohmann::json bankJson;

    try
    {
        file >> bankJson;
    }
    catch (const std::exception& e)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "DialogueBankManager.LoadDialogueFromFile: Error reading [%s]: %s", filePath.string().c_str(), e.what());
        return {};
    }

    if (!bankJson.is_array())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueBankManager.LoadDialogueFromFile: JSON at [%s] is not array.", filePath.string().c_str());
        return {};
    }

    std::vector<std::string> lines = bankJson.get<std::vector<std::string>>();

    if (randomize)
    {
        static std::minstd_rand g(static_cast<unsigned>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()
        ));
        std::shuffle(lines.begin(), lines.end(), g);
    }

    if (count >= 0 && count < lines.size())
    {
        lines.resize(count);
    }

    return lines;
}

std::vector<std::string> DialogueBankManager::GetLines(const nlohmann::json& sourceJSON)
{
    /*
        What this function expects:

        "sourceJSON": [ <lines> ]

        "sourceJSON": <line>

        "sourceJSON": {
            "bankFile": <file>,
            "count": 10,
            "randomize": true
        }
    
    */

    if (sourceJSON.is_array())
    {
        return sourceJSON.get<std::vector<std::string>>();
    }

    if (sourceJSON.is_string())
    {
        return { sourceJSON.get<std::string>() };
    }

    if (sourceJSON.is_object())
    {

        if (!sourceJSON.contains("bankFile") || !sourceJSON["bankFile"].is_string())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "DialogueBan.GetLines: Source JSON is object, but has no \"bankFile\" or is not string.");
            return {};
        }

        if (!sourceJSON.contains("count") || !sourceJSON["count"].is_number())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueBan.GetLines: Source JSON is object, but has no \"count\" or is not number. Defaulting to 1.");
        }

        if (!sourceJSON.contains("randomize") || !sourceJSON["randomize"].is_boolean())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueBan.GetLines: Source JSON is object, but has no \"randomize\" or is not number. Defaulting to 'false'.");
        }

        const GameConfiguration& config = Configuration::Get();
        const std::filesystem::path bankFilePath = config.paths.dialogueDirectory / JSONParser::ParseJSONPath(sourceJSON["bankFile"]);
        const int count = sourceJSON.value("count", 1);
        const bool randomize = sourceJSON.value("randomize", false);

        return DialogueBankManager::LoadDialogueFromFile(bankFilePath, count, randomize);
    }

    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueBan.GetLines: Source JSON is not array, string or object. Returning no lines.");
    return {};
}

std::vector<ResolvedChoice> DialogueBankManager::ParseAndExpandChoices(const nlohmann::json& choicesJson, bool randomizeAllChoices)
{
    /*
        What this function expects:

        Note that choicesJSON is the default JSON array.

        [
            {
                "bankFile": "responsesBad.json",
                "count": 3,
                "randomize": true,
                "actions": [...]
            },
            {
                "bankFile": "responsesGood.json",
                "count": 1,
                "randomize": true,
                "actions": [...]
            }
        ]

        [
            {
              "text": "I love AUST!!!",
              "actions": null
            },
            {
              "text": "I hate AUST grrrrh",
              "actions": [{ "exitGame": null }]
            }
        ]

    
    */

    std::vector<ResolvedChoice> result;
    if (!choicesJson.is_array())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueBankManager.ParseAndExpandChoices: Choices JSON is not array. Returning empty vector.");
        return result;
    }

    for (const json& item : choicesJson)
    {
        nlohmann::json actionsJson = nlohmann::json::array();
        if (item.contains("actions"))
        {
            if (item["actions"].is_array() || item["actions"].is_object())
            {
                actionsJson = item["actions"];
            }
            else if (!item["actions"].is_null())
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueBankManager.ParseAndExpandChoices: Choice entry has 'actions' but is not array, object or null.");
            }
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueBankManager.ParseAndExpandChoices: Choice entry has no 'actions' key. To supress this, put the key and null.");
        }

        std::vector<SystemAction> actions = JSONParser::ParseJSONSystemActions(actionsJson);

        if (item.contains("bankFile"))
        {
            std::vector<std::string> bankLines = DialogueBankManager::GetLines(item);
            for (const std::string& line : bankLines)
            {
                result.push_back(ResolvedChoice { 
                    .text = line, 
                    .actions = actions
                });
            }
        }
        else if (item.contains("text") && item["text"].is_string())
        {
            result.push_back(ResolvedChoice{
                .text = item.at("text").get<std::string>(),
                .actions = actions
            });
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueBankManager.ParseAndExpandChoices: Choice entry doesn't have 'text' (or is not string) or 'bankFile'.");
        }
    }

    if (randomizeAllChoices && !result.empty())
    {
        static std::minstd_rand g(static_cast<unsigned>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()
        ));
        std::shuffle(result.begin(), result.end(), g);
    }

    return result;
}

