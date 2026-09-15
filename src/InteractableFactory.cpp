#include <SDL3/SDL_log.h>

#include "InteractableFactory.h"

#include "DialogueChoiceInteractable.h"
#include "DialogueNarrationInteractable.h"
#include "Miscs.h"
#include "MemoryTracker.h"
#include "StringMiscs.h"

using json = nlohmann::json;

Interactable* InteractableFactory::CreateInteractableFromJSON(const json& interactableJSON)
{
    /**
        What this function expects (examples):

        "interactableJSON": {
            "type": "dialogueNarration",
            "params": {
                "lines": [
                    "First Line",
                    "Second Line"
                ],
                "onEnd": null
            }
        

        "interactableJSON": {
            "type": "dialogueNarration",
            "params": {
                "lines": [
                    "First Line",
                    "Second Line"
                ],
                "onEnd": {
                    "event": "eventX",
                    "params": {
                        <eventXparams>
                     }
                }
            }
        }

        "interactableJSON": {
            "type": "changeMap",
            "params": {
                "map": "mapName"
            }
        }
    */

    if (!interactableJSON.is_object())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "InteractableFactory.CreateInteractableFromJSON: Expected object, got non-object.");
        return nullptr;
    }

    if (!interactableJSON.contains("type") || !interactableJSON["type"].is_string())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "InteractableFactory.CreateInteractableFromJSON: Missing type or type is not string for interactableJSON.");
        return nullptr;
    }

    const std::string& type = interactableJSON.at("type").get_ref<const std::string&>();
    const std::string& typeLwr = StringMiscs::ToLower(type);

    if (!interactableJSON.contains("params") || !interactableJSON["params"].is_object())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "InteractableFactory.CreateInteractableFromJSON: Missing parameters or parameters are not objects for interactableJSON '%s'.", type.c_str());
        return nullptr;
    }

    const json& params = interactableJSON["params"];

    if (typeLwr == "dialoguenarration")
    {
        DialogueNarrationInteractable* interactable = ENG_NEW(DialogueNarrationInteractable);
        if (params.contains("lines") && params["lines"].is_array())
        {
            interactable->SetLines(params.at("lines").get<std::vector<std::string>>());
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "InteractableFactory.CreateInteractableFromJSON: [%s] doesn't have lines or not an array. Defauting to empty vector.", type.c_str());
        }

        if (params.contains("onEnd"))
        {
            if (!params["onEnd"].is_null())
            {
                interactable->SetCallback(CreateCallbackFromJSON(params["onEnd"]));
            }
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "InteractableFactory.CreateInteractableFromJSON: [%s] doesn't have a callback. Defauting to no-op.", type.c_str());
        }

        return interactable;
    }
    else if (typeLwr == "dialoguechoice")
    {
        DialogueChoiceInteractable* interactable = ENG_NEW(DialogueChoiceInteractable);

        std::vector<std::string> choiceTexts;
        std::vector<std::function<void()>> callbacks;

        if (params.contains("question") && params["question"].is_string())
        {
            interactable->SetQuestion(params.at("question").get_ref<const std::string&>());
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "InteractableFactory.CreateInteractableFromJSON: [%s] don't have a question or not a string. Defauting to empty string.", type.c_str());
        }

        if (params.contains("choices") && params["choices"].is_array())
        {
            for (const json& choice : params["choices"])
            {
                if (!choice.contains("text") || !choice["text"].is_string())
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "InteractableFactory.CreateInteractableFromJSON: [%s] doesn't contain text or is not a string. Defaulting to empty string.", type.c_str());
                }

                // Can't use reference because we need a copy regardless.
                std::string text = choice.value("text", "");

                choiceTexts.push_back(std::move(text));

                if (choice.contains("callback") && !choice["callback"].is_null())
                {
                    callbacks.push_back(CreateCallbackFromJSON(choice["callback"]));
                }
                else
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "InteractableFactory.CreateInteractableFromJSON: [%s] choice doesn't contain callback or is null. Defaulting to no-op.", type.c_str());
                    callbacks.push_back([](){});
                }
            }

            interactable->SetChoices(choiceTexts);
            interactable->SetCallbacks(callbacks);
        }
        else
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "InteractableFactory.CreateInteractableFromJSON: [%s] 'choices' is missing or not an array. Skipping choices setup.", type.c_str());
        }

        if (params.contains("onEnd"))
        {
            if (!params["onEnd"].is_null())
            {
                interactable->SetOnDialogueEndCallback(CreateCallbackFromJSON(params["onEnd"]));
            }
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "InteractableFactory.CreateInteractableFromJSON: [%s] doesn't have a callback. Defauting to no-op.", type.c_str());
        }


        return interactable;
    }

    // TODO: Add mapTransition, other things I come up with.

    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "InteractableFactory.CreateInteractableFromJSON: Unknown interactableJSON type: [%s]", type.c_str());
    return nullptr;
}