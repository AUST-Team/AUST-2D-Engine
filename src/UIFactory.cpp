#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>
#include <fstream>
#include <filesystem>

#include "UIFactory.h"

#include "Configuration.h"
#include "MemoryTracker.h"
#include "FileLoader.h"
#include "UIButton.h"
#include "UILabelMasked.h"
#include "UILabel.h"
#include "JSONParser.h"
#include "UIPanel.h"
#include "FontManager.h"
#include "UITextEffectSystem.h"
#include "StringMiscs.h"
#include "AppRenderSystem.h"
#include "ModManager.h"
#include "UITable.h"
#include "UILabelCheckbox.h"
#include "SystemValueRegistry.h"
#include "UISaveSlot.h"
#include "UICutsceneTextWriter.h"
#include "FloatUtils.h"
#include "Miscs.h"
#include "UIDialogue.h"
#include "TileRegistry.h"
#include "SDLMiscs.h"
#include "IOMiscs.h"
#include "UITextField.h"
#include "DataProvider.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

/**
* @brief Applies a style to a button.
* 
* @param [out] button Pointer to the button.
* @param stylePayload 
*/
static void ApplyStyleToButton(UIButton* button, const nlohmann::json& stylePayload);

/**
* @brief Applies a text effect key on an element.
* 
* @param [out] element Pointer to the element.
* @param text Text of the element.
* @param effectID ID of the effect.
* @param effectJSON JSON containing the effect data.
*/
static void ApplyTextEffectKey(UITextElement* element, const std::string& effectID, const json& effectJSON);

/**
* @brief Parses an UILabelMasked
* 
* @param maskedJSON JSON containing the masked label data.
* @param id ID of the element.
* 
* @return Pointer to the UILabelMasked, or nullptr.
*/
static UILabelMasked* ParseLabelMasked(const json& maskedJSON, const std::string& id);

/**
* @brief Parses an UILabel
*
* @param labelJSON JSON containing the label data.
* @param id ID of the element.
*
* @return Pointer to the UILabel, or nullptr.
*/
static UILabel* ParseLabel(const json& labelJSON, const std::string& id);

/**
* @brief Parses an UIButton
*
* @param buttonJSON JSON containing the button data.
* @param id ID of the element.
* @param actionCallback The callback function to be called when an action is triggered.
*
* @return Pointer to the UIButton, or nullptr.
*/
static UIButton* ParseButton(const json& buttonJSON, const std::string& id, std::function<void(const SystemAction&)> actionCallback);

/**
* @brief Parses an UIPanel.
* 
* @param panelJSON JSON containing the panel data.
* @param id ID of the element.
* @param actionCallback The callback function to be called when an action is triggered.
* 
* @return Pointer to the UIPanel created, or nullptr.
*/
static UIPanel* ParsePanel(const json& panelJSON, const std::string& id, std::function<void(const SystemAction&)> actionCallback);

/**
* @brief Parses an UITable.
*
* @param tableJSON JSON containing the table data.
* @param id ID of the element.
*
* @return Pointer to the UITable created, or nullptr.
*/
static UITable* ParseTable(const json& tableJSON, const std::string& id);

/**
* @brief Parses an UILabelCheckbox.
*
* @param checkboxJSON JSON containing the checkbox data.
* @param id ID of the element.
* @param actionCallback The callback function to be called when an action is triggered.
*
* @return Pointer to the UITable created, or nullptr.
*/
static UILabelCheckbox* ParseLabelCheckbox(const json& checkboxJSON, const std::string& id, std::function<void(const SystemAction&)> actionCallback);

/**
* @brief Parses an UISaveSlot.
* 
* @param slotJSON JSON containing the save slot data.
* @param id ID of the element.
* @param actionCallback The callback function to be called when an action is triggered.
* 
* @return Pointer to the UISaveSlot created, or nullptr.
*/
static UISaveSlot* ParseSaveSlot(const json& slotJSON, const std::string& id, std::function<void(const SystemAction&)> actionCallback);

/**
* @brief Parses a UICutsceneTextWriter.
* 
* @param cutsceneJSON JSON contining the cutscene writer data.
* @param id ID of the element.
* @param panel Pointer to the panel.
* @param actionCallback The callback function to be called when an action is triggered.
* 
* @return Pointer to the CutsceneWriter, or nullptr.
*/
static UICutsceneTextWriter* ParseCutsceneWriter(const json& cutsceneJSON, const std::string& id, UIPanel* panel, std::function<void(const SystemAction&)> actionCallback);

/**
* @brief Parses an UITextField.
* 
* @param fieldJSON JSON containing the text field data.
* @param id ID of the element.
* @param actionCallback The callback function to be called when an action is triggered.
* 
* @return Pointer to the UITextField, or nullptr.
*/
static UITextField* ParseTextField(const json& fieldJSON, const std::string& id, std::function<void(const SystemAction&)> actionCallback);

UIPanel* UIFactory::CreatePanelFromFile(const fs::path& filePath, const std::string& id, std::function<void(const SystemAction&)> actionCallback)
{
    std::optional<json> jsonOptional = FileLoader::LoadJSON(filePath);

    if (!jsonOptional)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreatePanelFromJSON: Failed to open JSON file: %s", filePath.string().c_str());
        return nullptr;
    }

    return CreatePanelFromJSON(*jsonOptional, id, actionCallback);
}

UIPanel* UIFactory::CreatePanelFromJSON(const json& panelJSON, const std::string& id, std::function<void(const SystemAction&)> actionCallback)
{
    /*
    
        What this function expects:

        Note that panelJSON is the main JSON object

        {
            "panel": {
                "backgroundColor": [ 50, 54, 62, 255 ],
                "borderColor": [0, 0, 0, 0]
            },
            "elements": [
                {
                    "type": "LabelMasked",
                    "id": "title_label",
                    "element": {
                        "bounds": { "x": 0, "y": 50, "w": 800, "h": 150 },
                        "backgroundColor": { "r": 0, "g": 0, "b": 0, "a": 0 },
                        "borderColor": { "r": 0, "g": 0, "b": 0, "a": 0 }
                    },
                    "textElement": {
                        "text": "AUST",
                        "effect": {"typewriter": {"delay": {"ms": 500} } }
                    },
                    "textStyle": {
                        "fontFamily": "title",
                        "fontSize": 150.0,
                        "fontColor": { "r": 255, "g": 255, "b": 255, "a": 255 },
                        "textAlignment": { "horizontal": "center", "vertical": "middle" }
                    },
                    "maskImage": "resources/images/flag.png"
                },
                {
                    "type": "Button",
                    "id": "start_button",
                    "element": {
                        "bounds": { "x": 300, "y": 250, "w": 200, "h": 50 },
                        "backgroundColor": { "r": 0, "g": 0, "b": 0, "a": 0 },
                        "borderColor": { "r": 0, "g": 0, "b": 0, "a": 0 }
                    },
                    "textElement": {
                        "text": "Start",
                        "effect": {"typewriter": {"delay": {"ms": 600} } }
                    },
                    "textStyle": {
                        "fontFamily": "button",
                        "fontSize": 24.0,
                        "fontColor": { "r": 255, "g": 255, "b": 255, "a": 255 },
                        "textAlignment": { "horizontal": "center", "vertical": "middle" }
                    },
                    "onClick": {
                        "system": "startGame"
                    },
                    "onMouseEnter": {
                        "changeStyle": {
                            "backgroundColor": [255, 0, 61, 255]
                        }
                    },
                    "onMouseExit": {
                        "changeStyle": {
                            "backgroundColor": [0, 0, 0, 0]
                        }
                    }
                }
                ...
            ]
        }
    */

    if (!panelJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreatePanelFromJSON: Panel JSON is not object.");
        return nullptr;
    }

    if (!panelJSON.contains("panel") || !panelJSON["panel"].is_object())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreatePanelFromJSON: Panel JSON doesn't contain 'panel' metadata, or is not object.");
    }

    UIPanel* panel = ParsePanel(panelJSON.value("panel", json::object()), id, actionCallback);

    if (!panelJSON.contains("elements") || !panelJSON["elements"].is_array())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreatePanelFromJSON: Panel doesn't contain elements. This could be intentional.");
        return panel;
    }

    for (const json& elemJSON : panelJSON["elements"])
    {
        if (!elemJSON.is_object())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreatePanelFromJSON: Element JSON is not object");
            continue;
        }

        if (!elemJSON.contains("type") || !elemJSON["type"].is_string())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreatePanelFromJSON: Element JSON doesn't contain type. Skipping.");
            continue;
        }

        if (!elemJSON.contains("id") || !elemJSON["id"].is_string())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreatePanelFromJSON: Element JSON doesn't contain id. Defaulting to empty string.");
        }

        const std::string& type = elemJSON.at("type").get_ref<const std::string&>();
        const std::string& typeLwr = StringMiscs::ToLower(type);
        std::string elemId = elemJSON.value("id", "");

        UIElement* element = nullptr;

        if (typeLwr == "label")
        {
            element = ParseLabel(elemJSON, elemId);
        }
        else if (typeLwr == "button")
        {
            element = ParseButton(elemJSON, elemId, actionCallback);
        }
        else if (typeLwr == "panel")
        {
            element = CreatePanelFromJSON(elemJSON, elemId, actionCallback);
        }
        else if (typeLwr == "table")
        {
            element = ParseTable(elemJSON, elemId);
        }
        else if (typeLwr == "labelcheckbox")
        {
            element = ParseLabelCheckbox(elemJSON, elemId, actionCallback);
        }
        else if (typeLwr == "saveslot")
        {
            element = ParseSaveSlot(elemJSON, elemId, actionCallback);
        }
        else if (typeLwr == "cutscenewriter")
        {
            element = ParseCutsceneWriter(elemJSON, elemId, panel, actionCallback);
        }
        else if (typeLwr == "labelmasked")
        {
            element = ParseLabelMasked(elemJSON, elemId);
        }
        else if (typeLwr == "textfield")
        {
            element = ParseTextField(elemJSON, elemId, actionCallback);
        }

        if (!element)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreatePanelFromJSON: Element JSON [%s] parsing returned null pointer.", elemId.c_str());
        }

        panel->AddElement(element);
    }

    return panel;
}

UIPanel* UIFactory::CreateUIFromFile(const fs::path& filePath, std::unordered_map<std::string, UIPanel*>& panels, std::function<void(const SystemAction&)> actionCallback)
{
    panels.clear();
    std::optional<json> jsonOptional = FileLoader::LoadJSON(filePath);

    if (!jsonOptional)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateUIFromFile: Failed to open JSON file: %s", filePath.string().c_str());
        return nullptr;
    }

    return CreateUIFromJSON(*jsonOptional, panels, actionCallback);
}

UIPanel* UIFactory::CreateUIFromJSON(const nlohmann::json& uiJSON, std::unordered_map<std::string, UIPanel*>& panels, std::function<void(const SystemAction&)> actionCallback)
{
    /*
        What this function expects:

        Note that panelJSON is the default JSON object

        {
          "mainPanel": "main",
          "panels": {
            "main": {
                "panel": {
                    "backgroundColor": [ 50, 54, 62, 255 ]
                },
                "elements": [
                    {
                        "type": "LabelMasked",
                        "id": "title_label",
                        "bounds": { "x": 0, "y": 50, "w": 800, "h": 150 },
                        "backgroundColor": { "r": 0, "g": 0, "b": 0, "a": 0 },
                        "borderColor": { "r": 255, "g": 255, "b": 255, "a": 255 },
                        "text": "AUST",
                        "fontFamily": "title",
                        "fontSize": 150.0,
                        "fontColor": { "r": 255, "g": 255, "b": 255, "a": 255 },
                        "textAlignment": { "horizontal": "center", "vertical": "middle" },
                        "maskImage": "resources/images/flag.png"
                        "effect": {"typewriter": {"delay": {"ms": 500} } }
                    },
                    {
                        "type": "Button",
                        "id": "start_button",
                        "bounds": { "x": 300, "y": 250, "w": 200, "h": 50 },
                        "backgroundColor": { "r": 0, "g": 0, "b": 0, "a": 0 },
                        "borderColor": { "r": 0, "g": 0, "b": 0, "a": 0 },
                        "text": "Start",
                        "font_type": "button",
                        "fontSize": 24.0,
                        "fontColor": { "r": 255, "g": 255, "b": 255, "a": 255 },
                        "textAlignment": { "horizontal": "center", "vertical": "middle" },
                        "effect": {"typewriter": {"delay": {"ms": 600} } },
                        "onClick": {
                            "system": "startGame"
                        },
                        "onMouseEnter": {
                            "changeStyle": {
                                "backgroundColor": [255, 0, 61, 255]
                            }
                        },
                        "onMouseExit": {
                            "changeStyle": {
                                "backgroundColor": [0, 0, 0, 0]
                            }
                        }
                    },
                    ...
                ]
            },
            "load": {
                "file": "load_panel.json"
            },
            "options": { ... }
          }
        }

    */
    
    panels.clear();

    if (!uiJSON.contains("panels"))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateUIFromJSON: Root element does not contain 'panels'.");
        return nullptr;
    }

    if (!uiJSON.contains("mainPanel") || !uiJSON["mainPanel"].is_string())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateUIFromJSON: Root element does not contain 'mainPanel' or is not string. Defaulting to \'main\'.");
    }

    std::string mainPanelKey = uiJSON.value("mainPanel", "main");
    UIPanel* mainPanelPtr = nullptr;

    const json& panelsSection = uiJSON["panels"];

    for (auto& [key, value] : panelsSection.items())
    {
        json finalPanelJSON;

        if (!value.is_object())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateUIFromJSON: Expected panel '%s' to be an object or file link.", key.c_str());
            continue;
        }

        if (value.contains("file"))
        {
            const std::string& filePath = value.at("file").get_ref<const std::string&>();
            std::optional<json> jsonOptional = FileLoader::LoadJSON(Configuration::Get().paths.uiDirectory / JSONParser::ParseJSONPath(value["file"]));

            if (!jsonOptional)
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateUIFromJSON: Failed to load external panel file: %s", filePath.c_str());
                continue;
            }

            finalPanelJSON = *jsonOptional;
        }
        else
        {
            finalPanelJSON = value;
        }

        UIPanel* newPanel = UIFactory::CreatePanelFromJSON(finalPanelJSON, key, actionCallback);

        if (newPanel)
        {
            panels[key] = newPanel;

            if (key == mainPanelKey)
            {
                mainPanelPtr = newPanel;
            }
        }
    }

    if (!mainPanelPtr)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateUIFromJSON: Main panel defined as '%s' was not loaded.", mainPanelKey.c_str());
    }

    return mainPanelPtr;
}

UIPanel* UIFactory::CreateUIByType(UIType type, std::unordered_map<std::string, UIPanel*>& panels, std::function<void(const SystemAction&)> actionCallback)
{
    panels.clear();
    std::optional<json> jsonOpt = FileLoader::LoadJSON(Configuration::Get().paths.uiFilePath, JSONParseFlags::None);

    if (!jsonOpt)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateUIByType: Failed to open JSON file: %s", Configuration::Get().paths.uiFilePath.string().c_str());
        return nullptr;
    }

    json uiJSON = *jsonOpt;

    if (!uiJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateUIByType: UI JSON is not object.");
        return nullptr;
    }

    const std::string* uiPath = nullptr;

    switch (type)
    {
        case UIType::MainMenu:
        {
            if (!uiJSON.contains("mainMenu") || !uiJSON["mainMenu"].is_string())
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateUIByType: UI JSON doesn't contain 'mainMenu' or is not string");
                return nullptr;
            }
            uiPath = uiJSON.at("mainMenu").get_ptr<const std::string*>();
            break;
        }
        case UIType::PauseMenu:
        {
            if (!uiJSON.contains("pauseMenu") || !uiJSON["pauseMenu"].is_string())
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateUIByType: UI JSON doesn't contain 'pauseMenu' or is not string");
                return nullptr;
            }
            uiPath = uiJSON.at("pauseMenu").get_ptr<const std::string*>();
            break;
        }
        default:
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateUIByType: Unknown UI type.");
            return nullptr;
        }
    }

    if (!uiPath)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateUIByType: UI path null.");
        return nullptr;
    }

    const fs::path filePath = Configuration::Get().paths.uiDirectory / IOMiscs::MakeCrossPlatformPath(*uiPath);

    return CreateUIFromFile(filePath, panels, actionCallback);
}

UIPanel* UIFactory::CreateCutsceneFromName(const std::string& cutsceneName, std::unordered_map<std::string, UIPanel*>& panels, std::function<void(const SystemAction&)> actionCallback)
{
    const fs::path filePath = Configuration::Get().paths.cutsceneDirectory / IOMiscs::MakeCrossPlatformPath(cutsceneName + ".json");
    return CreateUIFromFile(filePath, panels, actionCallback);
}

UIDialogue* UIFactory::CreateDialogue()
{
    std::optional<json> jsonOpt = FileLoader::LoadJSON(Configuration::Get().paths.uiFilePath, JSONParseFlags::None);
    if (!jsonOpt)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateDialogue: Failed to open JSON file: %s", Configuration::Get().paths.uiFilePath.string().c_str());
        return nullptr;
    }

    json uiJSON = *jsonOpt;

    if (!uiJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateDialogue: UI JSON is not object.");
        return nullptr;
    }
    
    if(!uiJSON.contains("dialogue") || !uiJSON["dialogue"].is_string())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateDialogue: UI JSON doesn't contain 'dialogue' or is not object.");
        return nullptr;
    }

    return CreateDialogueFromFile(Configuration::Get().paths.uiDirectory / JSONParser::ParseJSONPath(uiJSON["dialogue"]));
}

UIDialogue* UIFactory::CreateDialogueFromFile(const fs::path& filePath)
{
    std::optional<json> jsonOpt = FileLoader::LoadJSON(filePath);

    if (!jsonOpt)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateDialogueFromFile: Failed to open JSON file: %s", filePath.string().c_str());
        return nullptr;
    }

    json dialogueJSON = *jsonOpt;

    if (!dialogueJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateDialogueFromFile: Dialogue JSON is not object.");
        return nullptr;
    }

	return CreateDialogueFromJSON(dialogueJSON);
}

UIDialogue* UIFactory::CreateDialogueFromJSON(const json& dialogueJSON)
{
    /*
        What this function expects:

        Note that 'dialogueJSON' is the default JSON object.

        {
            "id": "dialogue",
            "element": {
                "bounds": { "x": 0, "y": 450, "w": 800, "h": 150 },
                "backgroundColor": { "r": 0, "g": 0, "b": 0, "a": 150 },
                "borderColor": { "r": 0, "g": 0, "b": 0, "a": 0 }
            },
            "textStyle": {
                "fontFamily": "dialogue",
                "fontSize": 20.0,
                "fontColor": { "r": 255, "g": 255, "b": 255, "a": 255 },
                "textAlignment": { "horizontal": "left", "vertical": "middle" }
            },
            "arrows": {
                "up": {
                    "name": "ArrowUp",
                    "alpha": 128
                },
                "down": {
                    "name": "ArrowDown",
                    "alpha": 128
                }
            },
            "blinkTime": {"ms": 1000.0},
            "padding": 40,
            "lineSpacing": 1.25,
            "iconSize": 36.0
        }
    */

    if (!dialogueJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateDialogueFromJSON: Dialogue JSON not object.");
        return nullptr;
    }

    if(!dialogueJSON.contains("id") || !dialogueJSON["id"].is_string())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateDialogueFromJSON: Dialogue JSON doesn't contain 'id' or is not string.");
        return nullptr;
	}

	const std::string& id = dialogueJSON.at("id").get_ref<const std::string&>();

    if (!dialogueJSON.contains("element") || !dialogueJSON["element"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateDialogueFromJSON: Dialogue JSON for [%s] doesn't contain 'element' or is not object.", id.c_str());
        return nullptr;
    }

    if (!dialogueJSON.contains("textStyle") || !dialogueJSON["textStyle"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateDialogueFromJSON: Dialogue JSON for [%s] doesn't contain 'textStyle' or is not object.", id.c_str());
        return nullptr;
    }

    std::array<DialogueArrow, enumCount<Direction>> arrows;

    if (dialogueJSON.contains("arrows") && dialogueJSON["arrows"].is_object())
    {
        for (const auto& [dir, arrowJSON] : dialogueJSON["arrows"].items())
        {
            if(!arrowJSON.is_object())
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateDialogueFromJSON: Dialogue JSON for [%s] has 'arrows' but direction [%s] is not object. Skipping.", id.c_str(), dir.c_str());
                continue;
			}

            if(!arrowJSON.contains("name") || !arrowJSON["name"].is_string())
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateDialogueFromJSON: Dialogue JSON for [%s] has 'arrows' but direction [%s] doesn't contain 'name' or is not string. Skipping.", id.c_str(), dir.c_str());
                continue;
			}

            if (!arrowJSON.contains("alpha") || !arrowJSON["alpha"].is_number())
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateDialogueFromJSON: Dialogue JSON for [%s] has 'arrows' but direction [%s] doesn't contain 'alpha' or is not number. Defaulting to '255'.", id.c_str(), dir.c_str());
            }

            Direction direction = DirectionFromString(dir);

            if (IsSentinel(direction))
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateDialogueFromJSON: Dialogue JSON for [%s] has 'arrows' but direction [%s] is not valid. Skipping.", id.c_str(), dir.c_str());
                continue;
            }

			const std::string& arrowName = arrowJSON.at("name").get_ref<const std::string&>();
            uint8_t alpha = arrowJSON.value("alpha", (uint8_t)255);

			const int tileIndex = TileRegistry::GetTileIndexByName(arrowName);

            if(tileIndex == ConstantConfiguration::invalidTileIndex)
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateDialogueFromJSON: Dialogue JSON for [%s] has 'arrows' but direction [%s] has invalid tile name [%s]. Skipping.", id.c_str(), dir.c_str(), arrowName.c_str());
                continue;
			}

			arrows[ToIndex(direction)] = DialogueArrow{.tileIndex = tileIndex, .transparency = alpha};
        }
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateDialogueFromJSON: Dialogue JSON for [%s] doesn't contain 'arrows' or is not object. Defaulting to none.", id.c_str());
    }
    
    float blinkTime = 1000.0f;

    if (dialogueJSON.contains("blinkTime"))
    {
        if (!dialogueJSON["blinkTime"].is_number() && !dialogueJSON["blinkTime"].is_object())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateDialogueFromJSON: Dialogue JSON for [%s] has 'blinkTime' but is not number or object. Defaulting to \"1000.0f\"", id.c_str());
        }
        else
        {
			blinkTime = JSONParser::ParseJSONTime<float>(dialogueJSON["blinkTime"], blinkTime);
        }
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateDialogueFromJSON: Dialogue JSON for [%s] does not contain 'blinkTime'. Defaulting to \"1000.0f\"", id.c_str());
    }

    SDL_FPoint padding { 40.0f, 20.0f };

    if(dialogueJSON.contains("padding") && (dialogueJSON["padding"].is_number() || dialogueJSON["padding"].is_object()))
    {
        padding = JSONParser::ParseJSONPadding(dialogueJSON["padding"], padding);
	}
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateDialogueFromJSON: Dialogue JSON for [%s] doesn't contain 'padding' or is not number or object. Defaulting to {40, 20}.", id.c_str());
    }

    if (!dialogueJSON.contains("lineSpacing") || !dialogueJSON["lineSpacing"].is_number())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateDialogueFromJSON: Dialogue JSON for [%s] doesn't contain 'lineSpacing' or is not number. Defaulting to 1.25.", id.c_str());
    }

    if (!dialogueJSON.contains("iconSize") || !dialogueJSON["iconSize"].is_number())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UIFactory.CreateDialogueFromJSON: Dialogue JSON for [%s] doesn't contain 'iconSize' or is not number. Defaulting to 36.0.", id.c_str());
    }

    UIElementProperties elementProps = JSONParser::ParseJSONUIElementProps(dialogueJSON["element"]);
    UITextStyleElement textStyle = JSONParser::ParseJSONTextStyle(dialogueJSON["textStyle"]);

	float lineSpacing = dialogueJSON.value("lineSpacing", 1.25f);
	float iconSize = dialogueJSON.value("iconSize", 36.0f);

    UIDialogue* dialogue = ENG_NEW(UIDialogue,
        id,
        elementProps.bounds,
        elementProps.backgroundColor,
        elementProps.borderColor,
        textStyle.GetTextFont(),
        textStyle.GetTextColor(),
        textStyle.GetTextAlignment(),
		textStyle.GetFontFamily(),
        arrows,
        blinkTime,
        padding,
        lineSpacing,
        iconSize
    );

    return dialogue;
}

static void ApplyTextEffectKey(UITextElement* element, const std::string& effectID, const json& effectJSON)
{
    /*
        What this function expects:

        Note that 'effectJSON' is the default JSON object.

        {
            "typewriter": {
                "delay": {
                    "ms": 600
                }
            }
        }
    */

    if (!element || !effectJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ApplyTextEffectKey: Element is NULL or JSON is not object.");
        return;
    }

    if (effectJSON.contains("typewriter") && effectJSON["typewriter"].contains("delay"))
    {
        float delay = JSONParser::ParseJSONTime<float>(effectJSON["typewriter"]["delay"], 0.0f);
        UITextEffectSystem::GetInstance().AddEffect(effectID, element->GetText(), delay);
        element->SetTextEffectKey(effectID);
    }
}

static void ApplyStyleToButton(UIButton* button, const nlohmann::json& stylePayload)
{
    /*
        What this function expects:

        Note that 'stylePayload' is the default JSON object.

        {
            "backgroundColor": [40, 44, 52, 255],
            "borderColor": [0, 0, 0, 0]
        }
    */

    if (!button || !stylePayload.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ApplyStyleToButton: Button is NULL or JSON is not object.");
        return;
    }

    if (stylePayload.contains("backgroundColor"))
    {
        SDL_Color bg = JSONParser::ParseJSONColor(stylePayload["backgroundColor"], SDL_Color { 0,0,0,0 });
        button->SetBackgroundColor(bg);
    }
    if (stylePayload.contains("borderColor"))
    {
        SDL_Color border = JSONParser::ParseJSONColor(stylePayload["borderColor"], SDL_Color { 255,255,255,255 });
        button->SetBorderColor(border);
    }
}

void UIFactory::AttachButtonActions(UIButton* button, const nlohmann::json& buttonJSON, std::function<void(const SystemAction&)> actionCallback)
{
    if (!button || !buttonJSON.is_object())
    {
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "AttachButtonActions: Button is NULL or JSON is not object.");
        return;
    }

    std::function<void(const json&, ButtonCallbackType)> registerEventCallbacks = [button, actionCallback](const nlohmann::json& eventJson, ButtonCallbackType callbackType) {
        std::vector<SystemAction> actions = JSONParser::ParseJSONSystemActions(eventJson);

        button->AddCallback([actionCallback, actions = std::move(actions), button]() {
            for (const SystemAction& action : actions)
            {
                if (action.type == "changeStyle")
                {
                    ApplyStyleToButton(button, action.payload);
                }
                else
                {
                    actionCallback(action);
                }
            }
        }, callbackType);
    };

    if (buttonJSON.contains("onClick"))
    {
        registerEventCallbacks(buttonJSON["onClick"], ButtonCallbackType::Click);
    }

    if (buttonJSON.contains("onMouseEnter"))
    {
        registerEventCallbacks(buttonJSON["onMouseEnter"], ButtonCallbackType::EnterHover);
    }

    if (buttonJSON.contains("onMouseExit"))
    {
        registerEventCallbacks(buttonJSON["onMouseExit"], ButtonCallbackType::ExitHover);
    }
}

UILabelMasked* ParseLabelMasked(const json& maskedJSON, const std::string& id)
{
    /*
        What this function expects:

        Note that maskedJSON is the default JSON object:

        {
            "element": {
                "bounds": { "x": 0, "y": 50, "w": 800, "h": 150 },
                "backgroundColor": { "r": 0, "g": 0, "b": 0, "a": 0 },
                "borderColor": { "r": 0, "g": 0, "b": 0, "a": 0 }
            },
            "textElement": {
                "text": "AUST",
                "effect": {"typewriter": {"delay": {"ms": 500} } }
            },
            "textStyle": {
                "fontFamily": "title",
                "fontSize": 150.0,
                "fontColor": { "r": 255, "g": 255, "b": 255, "a": 255 },
                "textAlignment": { "horizontal": "center", "vertical": "middle" }
            },
            "maskImage": "resources/images/flag.png"
        }
    */

    if (!maskedJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseLabelMasked: Masked label JSON for [%s] not object.", id.c_str());
        return nullptr;
    }

    if (!maskedJSON.contains("element") || !maskedJSON["element"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseLabelMasked: Masked label JSON for [%s] doesn't contain 'element' or is not object.", id.c_str());
        return nullptr;
    }

    if (!maskedJSON.contains("textElement") || !maskedJSON["textElement"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseLabelMasked: Masked label JSON for [%s] doesn't contain 'textElement' or is not object.", id.c_str());
        return nullptr;
    }

    if (!maskedJSON.contains("textStyle") || !maskedJSON["textStyle"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseLabelMasked: Masked label JSON for [%s] doesn't contain 'textStyle' or is not object.", id.c_str());
        return nullptr;
    }

    if (!maskedJSON.contains("maskImage") || !maskedJSON["maskImage"].is_string())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseLabelMasked: Masked label JSON for [%s] doesn't contain 'maskImage' or is not string.", id.c_str());
    }

    const json& textElement = maskedJSON["textElement"];

    if (!textElement.contains("text") || !textElement["text"].is_string())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseLabelMasked: Masked label JSON for [%s] doesn't contain 'text' or is not string.", id.c_str());
    }

    UIElementProperties elementProps = JSONParser::ParseJSONUIElementProps(maskedJSON["element"]);
    UITextStyleElement textStyle = JSONParser::ParseJSONTextStyle(maskedJSON["textStyle"]);

    const std::string& text = textElement.at("text").get_ref<const std::string&>();

    SDL_Texture* imageMask = nullptr;
    const fs::path maskImagePath = ModManager::GetInstance().ResolvePath(IOMiscs::MakeCrossPlatformPath(maskedJSON.value("maskImage", "")));
    imageMask = SDLMiscs::LoadTexture(maskImagePath, AppRenderSystem::GetInstance().GetRenderer());

    if (!imageMask)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseLabelMasked: Masked label JSON for [%s] failed to load mask image: %s.", id.c_str(), SDL_GetError());
        return nullptr;
    }

    UILabelMasked* label = ENG_NEW(UILabelMasked,
        id,
        elementProps.bounds, 
        elementProps.backgroundColor, 
        elementProps.borderColor, 
        text, 
        textStyle.GetTextFont(),
        textStyle.GetTextColor(), 
        textStyle.GetTextAlignment(), 
        textStyle.GetFontFamily(), 
        imageMask
    );

    if (maskedJSON.contains("textElement") && maskedJSON["textElement"].contains("effect"))
    {
        ApplyTextEffectKey(label, id, maskedJSON["textElement"]["effect"]);
    }

    return label;
}

UILabel* ParseLabel(const json& labelJSON, const std::string& id)
{
    /*
        What this function expects:

        Note that labelJSON is the default JSON object:

        {
            "element": {
                "bounds": { "x": 0, "y": 50, "w": 800, "h": 150 },
                "backgroundColor": { "r": 0, "g": 0, "b": 0, "a": 0 },
                "borderColor": { "r": 0, "g": 0, "b": 0, "a": 0 }
            },
            "textElement": {
                "text": "AUST",
                "effect": {"typewriter": {"delay": {"ms": 500} } }
            },
            "textStyle": {
                "fontFamily": "title",
                "fontSize": 150.0,
                "fontColor": { "r": 255, "g": 255, "b": 255, "a": 255 },
                "textAlignment": { "horizontal": "center", "vertical": "middle" }
            }
        },
    */

    if (!labelJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseLabel: Label JSON for [%s] not object.", id.c_str());
        return nullptr;
    }

    if (!labelJSON.contains("element") || !labelJSON["element"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseLabel: Label JSON for [%s] doesn't contain 'element' or is not object.", id.c_str());
        return nullptr;
    }

    if (!labelJSON.contains("textElement") || !labelJSON["textElement"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseLabel: Label JSON for [%s] doesn't contain 'textElement' or is not object.", id.c_str());
        return nullptr;
    }

    if (!labelJSON.contains("textStyle") || !labelJSON["textStyle"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseLabel: Label JSON for [%s] doesn't contain 'textStyle' or is not object.", id.c_str());
        return nullptr;
    }

    const json& textElement = labelJSON["textElement"];

    if (!textElement.contains("text") || !textElement["text"].is_string())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseLabel: Label JSON for [%s] doesn't contain 'text' or is not string.", id.c_str());
    }

    UIElementProperties elementProps = JSONParser::ParseJSONUIElementProps(labelJSON["element"]);
    UITextStyleElement textStyle = JSONParser::ParseJSONTextStyle(labelJSON["textStyle"]);

    const std::string& text = textElement.at("text").get_ref<const std::string&>();

    UILabel* label = ENG_NEW(UILabel,
        id,
        elementProps.bounds, 
        elementProps.backgroundColor, 
        elementProps.borderColor, 
        text, 
        textStyle.GetTextFont(),
        textStyle.GetTextColor(), 
        textStyle.GetTextAlignment(), 
        textStyle.GetFontFamily()
    );

    if (labelJSON.contains("textElement") && labelJSON["textElement"].contains("effect"))
    {
        ApplyTextEffectKey(label, id, labelJSON["textElement"]["effect"]);
    }

    return label;
}

UIButton* ParseButton(const json& buttonJSON, const std::string& id, std::function<void(const SystemAction&)> actionCallback)
{
    /*
        What this function expects:

        Note that buttonJSON is the default JSON object:

        {
            "element": {
                "bounds": { "x": 300, "y": 250, "w": 200, "h": 50 },
                "backgroundColor": { "r": 0, "g": 0, "b": 0, "a": 0 },
                "borderColor": { "r": 0, "g": 0, "b": 0, "a": 0 }
            },
            "textElement": {
                "text": "Start",
                "effect": {"typewriter": {"delay": {"ms": 600} } }
            },
            "textStyle": {
                "fontFamily": "button",
                "fontSize": 24.0,
                "fontColor": { "r": 255, "g": 255, "b": 255, "a": 255 },
                "textAlignment": { "horizontal": "center", "vertical": "middle" }
            },
            "onClick": {
                "system": "startGame"
            },
            "onMouseEnter": {
                "changeStyle": {
                    "backgroundColor": [255, 0, 61, 255]
                }
            },
            "onMouseExit": {
                "changeStyle": {
                    "backgroundColor": [0, 0, 0, 0]
                }
            }
        }
    */

    // I just finished watching Assassination Classroom.
    if (!buttonJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseButton: Button JSON for [%s] not object.", id.c_str());
        return nullptr;
    }

    if (!buttonJSON.contains("element") || !buttonJSON["element"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseButton: Button JSON for [%s] doesn't contain 'element' or is not object.", id.c_str());
        return nullptr;
    }

    if (!buttonJSON.contains("textElement") || !buttonJSON["textElement"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseButton: Button JSON for [%s] doesn't contain 'textElement' or is not object.", id.c_str());
        return nullptr;
    }

    if (!buttonJSON.contains("textStyle") || !buttonJSON["textStyle"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseButton: Button JSON for [%s] doesn't contain 'textStyle' or is not object.", id.c_str());
        return nullptr;
    }

    const json& textElement = buttonJSON["textElement"];

    if (!textElement.contains("text") || !textElement["text"].is_string())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseButton: Button JSON for [%s] doesn't contain 'text' or is not string.", id.c_str());
    }

    UIElementProperties elementProps = JSONParser::ParseJSONUIElementProps(buttonJSON["element"]);
    UITextStyleElement textStyle = JSONParser::ParseJSONTextStyle(buttonJSON["textStyle"]);

    const std::string& text = textElement.at("text").get_ref<const std::string&>();

    UIButton* button = ENG_NEW(UIButton,
        id,
        elementProps.bounds, 
        elementProps.backgroundColor, 
        elementProps.borderColor, 
        text, 
        textStyle.GetTextFont(),
        textStyle.GetTextColor(),
        textStyle.GetTextAlignment(),
        textStyle.GetFontFamily()
    );

	UIFactory::AttachButtonActions(button, buttonJSON, actionCallback);

    if (textElement.contains("effect"))
    {
        ApplyTextEffectKey(button, id, textElement["effect"]);
    }

    return button;
}

UIPanel* ParsePanel(const json& panelJSON, const std::string& id, std::function<void(const SystemAction&)> actionCallback)
{
    /*
        What this function expects:

        Note that panelJSON is the default JSON object:

        {
            "element": {
                "bounds": { "x": 0, "y": 0, "w": 800, "h": 600 },
                "backgroundColor": { "r": 50, "g": 54, "b": 62, "a": 255 },
                "borderColor": { "r": 0, "g": 0, "b": 0, "a": 0 }
            },
            "onEnter": null OR array OR object
            "onExit": null OR array OR object
        }
    */

    const GameConfiguration& config = Configuration::Get();
    float screenWidth = static_cast<float>(config.window.width);
    float screenHeight = static_cast<float>(config.window.height);

    UIPanel* panel = ENG_NEW(UIPanel,
        id,
        SDL_FRect { 0.0, 0.0, screenWidth, screenHeight },
        SDL_Color { 0, 0, 0, 255 },
        SDL_Color { 255, 255, 255, 255 }
    );

    if (!panelJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParsePanel: Panel JSON not object.");
        return panel;
    }

    if (!panelJSON.contains("element") || !panelJSON["element"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParsePanel: Panel JSON doesn't contain \"element\" or is not object.");
        return panel;
    }

    UIElementProperties element = JSONParser::ParseJSONUIElementProps(panelJSON["element"]);
    panel->SetBounds(element.bounds);
    panel->SetBackgroundColor(element.backgroundColor);
    panel->SetBorderColor(element.borderColor);

    if (panelJSON.contains("onEnter"))
    {
        if (panelJSON["onEnter"].is_object() || panelJSON["onEnter"].is_array())
        {
            std::vector<SystemAction> acts = JSONParser::ParseJSONSystemActions(panelJSON["onEnter"]);

            if (!acts.empty())
            {
                panel->AddCallback([actionCallback, actions = std::move(acts)]() {
                    for (const SystemAction& action : actions)
                    {
                        actionCallback(action);
                    }
                }, PanelCallbackType::PanelEnter);
            }
        }
        else if (!panelJSON["onEnter"].is_null())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParsePanel: Panel JSON contains \"onEnter\" but is not object, array or null.");
        }
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParsePanel: Panel JSON doesn't contain \"onEnter\". If you don't wish for callbacks, put the key and a null value.");
    }

    if (panelJSON.contains("onExit"))
    {
        if (panelJSON["onExit"].is_object() || panelJSON["onExit"].is_array())
        {
            std::vector<SystemAction> acts = JSONParser::ParseJSONSystemActions(panelJSON["onEnter"]);

            if (!acts.empty())
            {
                panel->AddCallback([actionCallback, actions = std::move(acts)]() {
                    for (const SystemAction& action : actions)
                    {
                        actionCallback(action);
                    }
                }, PanelCallbackType::PanelExit);
            }
        }
        else if (!panelJSON["onExit"].is_null())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParsePanel: Panel JSON contains \"onExit\" but is not object, array or null.");
        }
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParsePanel: Panel JSON doesn't contain \"onExit\". If you don't wish for callbacks, put the key and a null value.");
    }

    return panel;
}

UITable* ParseTable(const json& tableJSON, const std::string& id)
{
    /*
        What this function expects:

        Note that tableJSON is the default JSON object:

        {
            "element": {
                "bounds": { "x": 150, "y": 140, "w": 500, "h": 340 },
                "backgroundColor": { "r": 50, "g": 53, "b": 62, "a": 255 },
                "borderColor": { "r": 255, "g": 255, "b": 255, "a": 255 }
            },
            "header": {
                "fontFamily": "table",
                "fontSize": 20.0,
                "fontColor": { "r": 255, "g": 255, "b": 255, "a": 255 },
                "backgroundColor": { "r": 60, "g": 63, "b": 72 , "a": 255 }
            },
            "cell": {
                "fontFamily": "table",
                "fontSize": 18.0,
                "fontColor": { "r": 255, "g": 255, "b": 255, "a": 255 },
                "backgroundColor": { "r": 0, "g": 0, "b": 0 , "a": 0 }
            },
            "columns": [
                { "title": "Name", "width": 0.3, "textAlingment": {"horizontal": "left", "vertical": "middle" } },
                { "title": "Times Caught", "width": 0.4, "textAlingment": {"horizontal": "center", "vertical": "middle" } },
                { "title": "Time Played", "width": 0.3, "textAlingment": {"horizontal": "center", "vertical": "middle" } }
            ],
            "dataSource": {
                "type": "provider",
                "key": "topScores"
            },
            "rowHeight": 30.0,
            "padding": 5.0
        }
    */

    if (!tableJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseTable: Table JSON for [%s] not object.", id.c_str());
        return nullptr;
    }

    if (!tableJSON.contains("element") || !tableJSON["element"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseTable: Table JSON for [%s] doesn't contain 'element' or is not object.", id.c_str());
        return nullptr;
    }

    if (!tableJSON.contains("header") || !tableJSON["header"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseTable: Table JSON for [%s] doesn't contain 'header' or is not object.", id.c_str());
        return nullptr;
    }

    if (!tableJSON.contains("cell") || !tableJSON["cell"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseTable: Table JSON for [%s] doesn't contain 'cell' or is not object.", id.c_str());
        return nullptr;
    }

    UIElementProperties elementProps = JSONParser::ParseJSONUIElementProps(tableJSON["element"]);
    UITextStyleElement headerStyle = JSONParser::ParseJSONTextStyle(tableJSON["header"]);
    UITextStyleElement cellStyle = JSONParser::ParseJSONTextStyle(tableJSON["cell"]);

    SDL_Color headerBgColor { 0, 0, 0, 0 };
    const json& header = tableJSON["header"];

    if (header.contains("backgroundColor") && (header["backgroundColor"].is_object() || header["backgroundColor"].is_array() || header["backgroundColor"].is_string()))
    {
        headerBgColor = JSONParser::ParseJSONColor(header["backgroundColor"], headerBgColor);
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTable: Table JSON for [%s] header missing valid 'backgroundColor'. Defaulting to transparent.", id.c_str());
    }

    SDL_Color cellBgColor { 0, 0, 0, 0 };
    const json& cell = tableJSON["cell"];

    if (cell.contains("backgroundColor") && (cell["backgroundColor"].is_object() || cell["backgroundColor"].is_array() || cell["backgroundColor"].is_string()))
    {
        cellBgColor = JSONParser::ParseJSONColor(cell["backgroundColor"] , cellBgColor);
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTable: Table JSON for [%s] cell missing valid 'backgroundColor'. Defaulting to transparent.", id.c_str());
    }

    float rowHeight = 30.0f;
    if (tableJSON.contains("rowHeight") && tableJSON["rowHeight"].is_number())
    {
        rowHeight = tableJSON.at("rowHeight").get<float>();
    }
    else if (tableJSON.contains("rowHeight"))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTable: Table JSON for [%s] 'rowHeight' is not a number. Defaulting to 30.0.", id.c_str());
    }

    float padding = 5.0f;
    if (tableJSON.contains("padding") && tableJSON["padding"].is_number())
    {
        padding = tableJSON.at("padding").get<float>();
    }
    else if (tableJSON.contains("padding"))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTable: Table JSON for [%s] 'padding' is not a number. Defaulting to 5.0.", id.c_str());
    }

    UITable* table = ENG_NEW(UITable,
        id,
        elementProps.bounds,
        elementProps.backgroundColor,
        elementProps.borderColor,
        headerBgColor,
        cellBgColor,
        headerStyle,
        cellStyle,
        rowHeight,
        padding
    );

    if (tableJSON.contains("columns") && tableJSON["columns"].is_array())
    {
        std::vector<UITableColumn> columns;
        for (const json& colJSON : tableJSON["columns"])
        {
            if (!colJSON.is_object())
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTable: Table JSON for [%s] contains non-object column entry.", id.c_str());
                continue;
            }

            std::string title = colJSON.value("title", "");
            float width = colJSON.value("width", 0.0f);

            UITextAlignment alignment = { HorizontalAlignment::Center, VerticalAlignment::Middle };
            if (colJSON.contains("textAlignment") && colJSON["textAlignment"].is_object())
            {
                alignment = JSONParser::ParseJSONAlignment(colJSON["textAlignment"], alignment);
            }
            columns.push_back({ title, width, alignment });
        }
        table->SetTableColumns(columns);
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTable: Table JSON for [%s] missing 'columns' array.", id.c_str());
    }

    if (tableJSON.contains("dataSource") && tableJSON["dataSource"].is_object())
    {
        table->SetTableData(JSONParser::ParseJSONTableDatasource(tableJSON["dataSource"]));
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTable: Table JSON for [%s] missing 'dataSource' or is not object.", id.c_str());
    }

    return table;
}

UILabelCheckbox* ParseLabelCheckbox(const json& checkboxJSON, const std::string& id, std::function<void(const SystemAction&)> actionCallback)
{
    /*
        What this function expects:

        Note that checkboxJSON is the default JSON object.

        {
            "element": {
                "bounds": { "x": 300, "y": 80, "w": 200, "h": 30 },
                "backgroundColor": { "r": 0, "g": 0, "b": 0, "a": 0 },
                "borderColor": { "r": 0, "g": 0, "b": 0, "a": 0 }
            },
            "textElement": {
                "text": "Fullscreen"
            },
            "textStyle": {
                "fontFamily": "button",
                "fontSize": 24.0,
                "fontColor": { "r": 255, "g": 255, "b": 255, "a": 255 },
                "textAlignment": { "horizontal": "center", "vertical": "middle" }
            },
            "boxColor": [200, 200, 200, 255],
            "checkColor": [ 50, 200, 50, 255 ],
            "boxSize": 20.0,
            "spacing": 10.0,
            "initial": { "system": "isFullscreen" },
            "onToggle": {
                "system": "toggleFullscreen"
            }
        }
    */

    if (!checkboxJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseLabelCheckbox: Label checkbox JSON for [%s] not object.", id.c_str());
        return nullptr;
    }

    if (!checkboxJSON.contains("element") || !checkboxJSON["element"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseLabelCheckbox: Label checkbox JSON for [%s] doesn't contain 'element' or is not object.", id.c_str());
        return nullptr;
    }

    if (!checkboxJSON.contains("textElement") || !checkboxJSON["textElement"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseLabelCheckbox: Label checkbox JSON for [%s] doesn't contain 'textElement' or is not object.", id.c_str());
        return nullptr;
    }

    if (!checkboxJSON.contains("textStyle") || !checkboxJSON["textStyle"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseLabelCheckbox: Label checkbox JSON for [%s] doesn't contain 'textStyle' or is not object.", id.c_str());
        return nullptr;
    }

    const json& textElement = checkboxJSON["textElement"];

    if (!textElement.contains("text") || !textElement["text"].is_string())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseLabelCheckbox: Label checkbox JSON for [%s] doesn't contain 'text' or is not string.", id.c_str());
    }

    UIElementProperties elementProps = JSONParser::ParseJSONUIElementProps(checkboxJSON["element"]);
    UITextStyleElement textStyle = JSONParser::ParseJSONTextStyle(checkboxJSON["textStyle"]);

    const std::string& text = textElement.at("text").get_ref<const std::string&>();

    SDL_Color boxColor = { 200, 200, 200, 255 };
    if (checkboxJSON.contains("boxColor") && (checkboxJSON["boxColor"].is_object() || checkboxJSON["boxColor"].is_array() || checkboxJSON["boxColor"].is_string()))
    {
        boxColor = JSONParser::ParseJSONColor(checkboxJSON["boxColor"], boxColor);
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseLabelCheckbox: Label checkbox JSON for [%s] 'boxColor' field missing is not an object, array or string.", id.c_str());
    }

    SDL_Color checkColor = { 50, 200, 50, 255 };
    if (checkboxJSON.contains("checkColor") && (checkboxJSON["checkColor"].is_object() || checkboxJSON["checkColor"].is_array() || checkboxJSON["checkColor"].is_string()))
    {
        checkColor = JSONParser::ParseJSONColor(checkboxJSON["checkColor"], checkColor);
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseLabelCheckbox: Label checkbox JSON for [%s] 'checkColor' field missing is not an object, array or string.", id.c_str());
    }

    CheckboxState initialChecked = CHECKBOX_UNCHECKED;
    if (checkboxJSON.contains("binding") && checkboxJSON["binding"].is_object())
    {
        const json& binding = checkboxJSON["binding"];
        std::string type = binding.value("type", "");
        std::string key = binding.value("key", "");

        if (type == "system")
        {
            initialChecked = SystemValueRegistry::GetInstance().GetValue<CheckboxState>(key, false);
        }
    }
    else if (checkboxJSON.contains("initial") && checkboxJSON["initial"].is_boolean())
    {
        initialChecked = checkboxJSON.at("initial").get<CheckboxState>();
    }

    float boxSize = 20.0f;
    if (checkboxJSON.contains("boxSize") && checkboxJSON["boxSize"].is_number())
    {
        boxSize = checkboxJSON.at("boxSize").get<float>();
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseLabelCheckbox: Label checkbox JSON for [%s] 'boxSize' missing or is not a number.", id.c_str());
    }

    float spacing = 10.0f;
    if (checkboxJSON.contains("spacing") && checkboxJSON["spacing"].is_number())
    {
        spacing = checkboxJSON.at("spacing").get<float>();
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseLabelCheckbox: Label checkbox JSON for [%s] 'spacing' missing or is not a number.", id.c_str());
    }

    UILabelCheckbox* labelCheckbox = ENG_NEW(UILabelCheckbox,
        id,
        elementProps.bounds,
        elementProps.backgroundColor,
        elementProps.borderColor,
        text,
        textStyle.GetTextFont(),
        textStyle.GetTextColor(),
        textStyle.GetTextAlignment(),
        textStyle.GetFontFamily(),
        boxColor,
        checkColor,
        initialChecked,
        boxSize,
        spacing
    );

    if (checkboxJSON.contains("onToggle"))
    {
        std::vector<SystemAction> toggleActions = JSONParser::ParseJSONSystemActions(checkboxJSON["onToggle"]);

        if (!toggleActions.empty())
        {
            labelCheckbox->SetOnToggle([actionCallback, actions = std::move(toggleActions)](bool checked) {
                for (const SystemAction& act : actions)
                {
                    SystemAction runtimeAction = act;

                    if (runtimeAction.type == "toggleFullscreen" && (runtimeAction.payload.is_null() || runtimeAction.payload.is_boolean()))
                    {
                        runtimeAction.payload = checked;
                    }

                    actionCallback(runtimeAction);
                }
            });
        }
    }

    return labelCheckbox;
}

UISaveSlot* ParseSaveSlot(const json& slotJSON, const std::string& id, std::function<void(const SystemAction&)> actionCallback)
{
    /*
        What this function expects:

        Note that slotJSON is the default JSON object.

        {
            "element": {
                "bounds": {"x": 150, "y": "_STARTING_Y", "w": 500, "h": "_SLOT_HEIGHT"},
                "backgroundColor": { "r": 0, "g": 0, "b": 0, "a": 0 },
                "borderColor": { "r": 255, "g": 255, "b": 255, "a": 255 }
            },
            "textStyle": {
                "fontFamily": "save",
                "fontSize": 16.0,
                "fontColor": { "r": 255, "g": 255, "b": 255, "a": 255 },
                "textAlignment": { "horizontal": "center", "vertical": "middle" }
            },
            "mode": "load",
            "saveId": 1,
            "buttons": {
                "onMouseEnter": {
                    "changeStyle": { "backgroundColor": [255, 0, 61, 255] }
                },
                "onMouseExit": {
                    "changeStyle": { "backgroundColor": [0, 0, 0, 0] }
                },
                "onClick": {
                    "playAudio": "button_click"
                }
            }
        }
    */

    if (!slotJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseSaveSlot: Save slot JSON for [%s] not object.", id.c_str());
        return nullptr;
    }

    if (!slotJSON.contains("element") || !slotJSON["element"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseSaveSlot: Save slot JSON for [%s] doesn't contain 'element' or is not object.", id.c_str());
        return nullptr;
    }

    if (!slotJSON.contains("textStyle") || !slotJSON["textStyle"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseSaveSlot: Save slot JSON for [%s] doesn't contain 'textStyle' or is not object.", id.c_str());
        return nullptr;
    }

    if (!slotJSON.contains("mode") || !slotJSON["mode"].is_string())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseSaveSlot: Save slot JSON for [%s] doesn't contain 'mode' or is not string. Defaulting to 'save'", id.c_str());
    }

    if (!slotJSON.contains("saveId") || !slotJSON["saveId"].is_number_integer())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseSaveSlot: Save slot JSON for [%s] doesn't contain 'saveId' or is not integer. Defaulting to '1'", id.c_str());
    }

    std::string modeStr = slotJSON.value("mode", "save");
    int saveId = slotJSON.value("saveId", 1);
    UIElementProperties elementProps = JSONParser::ParseJSONUIElementProps(slotJSON["element"]);
    UITextStyleElement textStyle = JSONParser::ParseJSONTextStyle(slotJSON["textStyle"]);

    SaveSlotMode mode = SaveSlotModeFromString(modeStr);

    if (IsSentinel(mode))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseSaveSlot: Save slot JSON for [%s] failed to save slot mode. Defaulting to 'save'", id.c_str());
        mode = SaveSlotMode::Save;
    }

    UISaveSlot* slot = ENG_NEW(UISaveSlot,
        id,
        elementProps.bounds,
        elementProps.backgroundColor,
        elementProps.borderColor,
        textStyle.GetTextFont(),
        textStyle.GetTextColor(),
        textStyle.GetTextAlignment(),
        textStyle.GetFontFamily(),
        saveId,
        mode
    );

    if (slotJSON.contains("buttons"))
    {
        if (slotJSON["buttons"].is_object())
        {
            slot->ApplyButtonConfigurations(slotJSON["buttons"], actionCallback);
        }
        else if (!slotJSON["buttons"].is_null())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseSaveSlot: Save slot JSON for [%s] contains \"buttons\" but is not object and is not null.", id.c_str());

        }
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseSaveSlot: Save slot JSON for [%s] does not have \"buttons\"", id.c_str());
    }

    return slot;
}

UICutsceneTextWriter* ParseCutsceneWriter(const json& cutsceneJSON, const std::string& id, UIPanel* panel, std::function<void(const SystemAction&)> actionCallback)
{
    /*
        What this function expects:

        Note that cutsceneJSON is the default JSON object.

        {
            "element": {
                "bounds": { "x": 0, "y": 0, "w": 800, "h": 600 },
                "backgroundColor": { "r": 0, "g": 0, "b": 0, "a": 0 },
                "borderColor": { "r": 0, "g": 0, "b": 0, "a": 0 }
            },
            "textStyle": {
                "fontFamily": "cutscene",
                "fontSize": 35.0,
                "fontColor": { "r": 255, "g": 255, "b": 255, "a": 150 },
                "textAlignment": { "horizontal": "center", "vertical": "middle" }
            },
            "pauseTime": { "ms": 2000.0 },
            "fadeSpeed": 0.01,
            "typewriterSpeed": { "ms": 40.0 },
            "skipLabel": "skip_label",
            "lines": [
                "The war ended. Your country, Faloria, is in chaos.",
                "After several power struggles, King Patrick takes control over the country.",
                "Famine ravages the country, which makes the king paranoid about a possible coup.",
                "Due to this, rationing is introduced, coupled with repossessing farmers' lands.",
                "They call this program \"AUST\".",
                "A curfew is imposed, enforced by the king's royal guards, the so called \"Unity Guards\".",
                "You are a mere farmer. And now you work everyday on land that was once yours.",
                "Perhaps, one day, your country will return to the pre-war democratic regime.",
                "Or perhaps, you will use this opportunity to lift yourself up the social ladder.",
                "The choice is yours."
            ]
        }
    */

    if (!cutsceneJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseCutsceneWriter: Cutscene writer JSON for [%s] not object.", id.c_str());
        return nullptr;
    }

    if (!cutsceneJSON.contains("element") || !cutsceneJSON["element"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseCutsceneWriter: Cutscene writer JSON for [%s] doesn't contain 'element' or is not object.", id.c_str());
        return nullptr;
    }

    if (!cutsceneJSON.contains("textStyle") || !cutsceneJSON["textStyle"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseCutsceneWriter: Cutscene writer JSON for [%s] doesn't contain 'textStyle' or is not object.", id.c_str());
        return nullptr;
    }

    if (!cutsceneJSON.contains("lines") || !cutsceneJSON["lines"].is_array())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseCutsceneWriter: Cutscene writer JSON for [%s] doesn't contain 'lines' or is not array. Why even make a cutscene writer with no lines?", id.c_str());
        return nullptr;
    }

    float pauseTime = 2000.0f;

    if (cutsceneJSON.contains("pauseTime"))
    {
        if (!cutsceneJSON["pauseTime"].is_number() && !cutsceneJSON["pauseTime"].is_object())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseCutsceneWriter: Cutscene writer JSON for [%s] has 'pauseTime' but is not number or object. Defaulting to \"2000.0f\"", id.c_str());
        }
        else
        {
            pauseTime = JSONParser::ParseJSONTime<float>(cutsceneJSON["pauseTime"], pauseTime);
        }
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseCutsceneWriter: Cutscene writer JSON for [%s] does not contain 'pauseTime'. Defaulting to \"2000.0f\"", id.c_str());
    }

    if (!cutsceneJSON.contains("fadeSpeed") || !cutsceneJSON["fadeSpeed"].is_number_float())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseCutsceneWriter: Cutscene writer JSON for [%s] doesn't contain 'fadeSpeed' or is not float. Defulting to \'0.01\'", id.c_str());
    }

    float typewriterSpeed = 40.0f;

    if (cutsceneJSON.contains("typewriterSpeed") && (cutsceneJSON["typewriterSpeed"].is_object() || cutsceneJSON["typewriterSpeed"].is_number()))
    {
        typewriterSpeed = JSONParser::ParseJSONTime<float>(cutsceneJSON["typewriterSpeed"], typewriterSpeed);
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseCutsceneWriter: Cutscene writer JSON for [%s] doesn't contain 'typewriterSpeed' or is not number / object. Defulting to \'40\'", id.c_str());
    }

    if (!cutsceneJSON.contains("skipLabel") || !cutsceneJSON["skipLabel"].is_string())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseCutsceneWriter: Cutscene writer JSON for [%s] doesn't contain 'skipLabel' or is not string. Defulting to empty string", id.c_str());
    }

    UIElementProperties elementProps = JSONParser::ParseJSONUIElementProps(cutsceneJSON["element"]);
    UITextStyleElement textStyle = JSONParser::ParseJSONTextStyle(cutsceneJSON["textStyle"]);

    float fadeSpeed = cutsceneJSON.value("fadeSpeed", 0.01f);

    const std::string skipLabelId = cutsceneJSON.value("skipLabel", "");
    std::vector<std::string> lines = cutsceneJSON.at("lines").get<std::vector<std::string>>();

    UICutsceneTextWriter* cutsceneWriter = ENG_NEW(UICutsceneTextWriter,
        id,
        elementProps.bounds,
        elementProps.backgroundColor,
        elementProps.borderColor,
        textStyle.GetTextFont(),
        textStyle.GetTextColor(),
        textStyle.GetTextAlignment(),
        textStyle.GetFontFamily(),
        pauseTime,
        fadeSpeed,
        typewriterSpeed,
        lines
    );

    if (!skipLabelId.empty() && panel)
    {
        if (UILabel* skipLabel = panel->FindElementById<UILabel>(skipLabelId))
        {
            cutsceneWriter->SetSkipLabel(skipLabel);
        }
    }

    if (cutsceneJSON.contains("onComplete") && cutsceneJSON["onComplete"].is_object())
    {
        std::vector<SystemAction> onCompleteActions = JSONParser::ParseJSONSystemActions(cutsceneJSON["onComplete"]);

        if (!onCompleteActions.empty())
        {
            cutsceneWriter->SetOnComplete([actionCallback, actions = std::move(onCompleteActions)]() {
                for (const SystemAction& act : actions)
                {
                    actionCallback(act);
                }
            });
        }
    }

    return cutsceneWriter;
}

UITextField* ParseTextField(const json& fieldJSON, const std::string& id, std::function<void(const SystemAction&)> actionCallback)
{
    /*
        What this function expects:

        Note that fieldJSON is the default JSON object:

        {
            "type": "TextField",
            "element": {
                "bounds": { "x": 240, "y": 275, "w": 320, "h": 40 },
                "backgroundColor": { "r": 0, "g": 0, "b": 0, "a": 0 },
                "borderColor": { "r": 0, "g": 0, "b": 0, "a": 0 }
            },
            "textStyle": {
                "fontFamily": "label",
                "fontSize": 20.0,
                "fontColor": { "r": 255, "g": 255, "b": 255, "a": 255 },
                "textAlignment": { "horizontal": "center", "vertical": "middle" }
            },
            "placeholderText": "(Leave blank to skip)",
            "maxLength": 32,
            "numericOnly": false,
            "showSlider": false,
            "blinkTime": { "ms": 500.0 },
            "onTextInputStart": [...],
            "onTextInputStop": [...]
        }
    */

    if (!fieldJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] not object.", id.c_str());
        return nullptr;
    }

    if (!fieldJSON.contains("element") || !fieldJSON["element"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] doesn't contain 'element' or is not object.", id.c_str());
        return nullptr;
    }

    if (!fieldJSON.contains("textStyle") || !fieldJSON["textStyle"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] doesn't contain 'textStyle' or is not object.", id.c_str());
        return nullptr;
    }

    if (!fieldJSON.contains("maxLength") || !fieldJSON["maxLength"].is_number())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] doesn't contain 'maxLength' or is not number. Defaulting to \"32\"", id.c_str());
    }

    if (!fieldJSON.contains("numericOnly") || !fieldJSON["numericOnly"].is_boolean())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] doesn't contain 'numericOnly' or is not boolean. Defaulting to \"false\"", id.c_str());
    }

    if (!fieldJSON.contains("showSlider") || !fieldJSON["showSlider"].is_boolean())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] doesn't contain 'showSlider' or is not boolean. Defaulting to \"false\"", id.c_str());
    }

    if (!fieldJSON.contains("textPadding") || !fieldJSON["textPadding"].is_number())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] doesn't contain 'textPadding' or is not float. Defaulting to \"5.0f\"", id.c_str());
    }

    float blinkTime = 500.0f;

    if (fieldJSON.contains("blinkTime") && (fieldJSON["blinkTime"].is_object() || fieldJSON["blinkTime"].is_number()))
    {
        blinkTime = JSONParser::ParseJSONTime<float>(fieldJSON["blinkTime"], blinkTime);
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] doesn't contain 'blinkTime' or is not number of boolean. Defaulting to \"500.0 ms\"", id.c_str());
    }

    std::string placeholderText = "";

    if (fieldJSON.contains("placeholderText"))
    {
        if (fieldJSON["placeholderText"].is_string())
        {
            placeholderText = fieldJSON.at("placeholderText").get<std::string>();
        }
        else if(!fieldJSON["placeholderText"].is_null())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] contains 'placeholderText' but is not string or null.", id.c_str());
        }
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] does not contain 'placeholderText'. If not used, put the key in with a null value to supress this.", id.c_str());
    }

    std::string dataBindKey = "";

    if (fieldJSON.contains("dataBindKey"))
    {
        if (fieldJSON["dataBindKey"].is_string())
        {
            dataBindKey = fieldJSON.at("dataBindKey").get<std::string>();
        }
        else if(!fieldJSON["dataBindKey"].is_null())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] contains 'dataBindKey' but is not string or null.", id.c_str());
        }
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] does not contain 'dataBindKey'. If not used, put the key in with a null value to supress this.", id.c_str());
    }

    std::string text = "";

    if (fieldJSON.contains("textElement"))
    {
        const json& textElement = fieldJSON["textElement"];
        if (textElement.is_object())
        {
            if (textElement.contains("text") && textElement["text"].is_string())
            {
                text = textElement.at("text").get<std::string>();
            }
            else
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] contains 'textElement' but doesn't contain 'text' or is not string.", id.c_str());
            }
        }
        else if (!textElement.is_null())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] contains 'textElement' but is not string or null.", id.c_str());
        }
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] does not contain 'textElement'. If not used, put the key in with a null value to supress this.", id.c_str());
    }

    SDL_Color cursorColor = { 255, 255, 255, 255 };

    if (fieldJSON.contains("cursorColor") && (fieldJSON["cursorColor"].is_array() || fieldJSON["cursorColor"].is_object()))
    {
        cursorColor = JSONParser::ParseJSONColor(fieldJSON["cursorColor"], cursorColor);
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] does not contain 'cursorColor', or is not object or array. Defaulting to white.", id.c_str());
    }

    UIElementProperties elementProps = JSONParser::ParseJSONUIElementProps(fieldJSON["element"]);
    UITextStyleElement textStyle = JSONParser::ParseJSONTextStyle(fieldJSON["textStyle"]);
    const size_t maxLength = fieldJSON.value("maxLength", 32);
    const bool numericOnly = fieldJSON.value("numericOnly", false);
    const bool showSlider = fieldJSON.value("showSlider", false);
    const float textPadding = fieldJSON.value("textPadding", 5.0f);

    UITextField* textField = ENG_NEW(UITextField,
        id,
        elementProps.bounds,
        elementProps.backgroundColor,
        elementProps.borderColor,
        text,
        textStyle.GetTextFont(),
        textStyle.GetTextColor(),
        textStyle.GetTextAlignment(),
        textStyle.GetFontFamily(),
        placeholderText,
        maxLength,
        blinkTime,
        numericOnly,
        showSlider,
        textPadding,
        cursorColor
    );

    if (!dataBindKey.empty())
    {
        textField->AddCallback([textField, dataBindKey]() {

            const std::string textValue = textField->GetText();

            if (!textValue.empty())
            {
                DataProvider::GetInstance().RegisterProvider( dataBindKey, [textValue]() {
                    return textValue;
                }, ProviderLifetime::OneTimeUse );
            }

        }, TextFieldCallbackType::TextInputStop);
    }

    if (fieldJSON.contains("onTextInputStart"))
    {
        if (fieldJSON["onTextInputStart"].is_object() || fieldJSON["onTextInputStart"].is_array())
        {
            std::vector<SystemAction> textInputActions = JSONParser::ParseJSONSystemActions(fieldJSON["onTextInputStart"]);

            if (!textInputActions.empty())
            {
                textField->AddCallback([actionCallback, actions = std::move(textInputActions)]() {
                    for (const SystemAction& action : actions)
                    {
                        actionCallback(action);
                    }
                }, TextFieldCallbackType::TextInputStart);
            }
        }
        else if (!fieldJSON["onTextInputStart"].is_null())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] contains 'onTextInputStart' but is not object, array or null.", id.c_str());
        }
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] does not contain 'onTextInputStart'. If not used, put the key in with a null value to supress this.", id.c_str());
    }

    if (fieldJSON.contains("onTextInputStop"))
    {
        if (fieldJSON["onTextInputStop"].is_object() || fieldJSON["onTextInputStop"].is_array())
        {
            std::vector<SystemAction> textInputActions = JSONParser::ParseJSONSystemActions(fieldJSON["onTextInputStop"]);

            if (!textInputActions.empty())
            {
                textField->AddCallback([textField, actionCallback, actions = std::move(textInputActions)]() {
                    for (const SystemAction& action : actions)
                    {
                        SystemAction runtimeAction = action;

                        if (runtimeAction.type == "setMasterVolume" && textField->GetNumericOnlyFlag())
                        {
                            int val = textField->GetTextAsInteger();
                            if (val < 0)
                            {
                                continue;
                            }
                            runtimeAction.payload = val;
                        }

                        actionCallback(runtimeAction);
                    }
                }, TextFieldCallbackType::TextInputStop);
            }
        }
        else if (!fieldJSON["onTextInputStop"].is_null())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] contains 'onTextInputStop' but is not object, array or null.", id.c_str());
        }
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] does not contain 'onTextInputStop'. If not used, put the key in with a null value to supress this.", id.c_str());
    }

    if (fieldJSON.contains("binding"))
    {
        if (fieldJSON["binding"].is_object())
        {
            const json& dataSource = fieldJSON["binding"];

            if (dataSource.contains("type") && dataSource["type"].is_string() && dataSource.contains("key") && dataSource["key"].is_string())
            {
                const float masterVolumeNorm = SystemValueRegistry::GetInstance().GetValue(dataSource.at("key").get_ref<const std::string&>(), 1.0f);
                const int masterVolume = static_cast<int>(std::round(masterVolumeNorm * 100.0f));

                textField->SetText(std::to_string(masterVolume));
            }
        }
        else if (!fieldJSON["dataSource"].is_null())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] contains 'dataSource', but is not object or null.", id.c_str());
        }
        
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] missing 'dataSource' or is not object. Please put key and null to supress this.", id.c_str());
    }

    const bool containsSliderKey = fieldJSON.contains("slider");
    const bool isSliderObject = containsSliderKey && fieldJSON["slider"].is_object();
    const bool isSliderNull = !containsSliderKey || fieldJSON["slider"].is_null();

    const bool bothFlagsTrue = numericOnly && showSlider;
    const bool bothFlagsFalse = !numericOnly && !showSlider;

    if (isSliderObject && !bothFlagsTrue)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] has 'slider' object, but 'numericOnly' or 'showSlider' is false.", id.c_str());
    }
    else if (!isSliderObject && bothFlagsTrue)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] has 'numericOnly' and 'showSlider' as true, but 'slider' is missing or not an object.", id.c_str());
    }
    else if (bothFlagsFalse && !isSliderNull)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] has both flags as false, but 'slider' is provided and not null.", id.c_str());
    }

    if (containsSliderKey && isSliderObject && bothFlagsTrue)
    {
        const json& sliderJSON = fieldJSON["slider"];

        SDL_Color sliderTrackColor = { 80, 80, 80, 255 };
        SDL_Color sliderActiveColor = { 0, 180, 255, 255 };
        SDL_Color handleColor = { 240, 240, 240, 255 };
        FSize sliderSize = { 120.0f, 4.0f };
        FSize handleSize = { 1.0f, 16.0f };

        if (sliderJSON.contains("sliderTrackColor") && (sliderJSON["sliderTrackColor"].is_array() || sliderJSON["sliderTrackColor"].is_object()))
        {
            sliderTrackColor = JSONParser::ParseJSONColor(sliderJSON["sliderTrackColor"], sliderTrackColor);
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] does not contain 'sliderTrackColor', or is not object or array. Defaulting to { 80, 80, 80, 255 }.", id.c_str());
        }

        if (sliderJSON.contains("sliderActiveColor") && (sliderJSON["sliderActiveColor"].is_array() || sliderJSON["sliderActiveColor"].is_object()))
        {
            sliderActiveColor = JSONParser::ParseJSONColor(sliderJSON["sliderActiveColor"], sliderActiveColor);
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] does not contain 'sliderActiveColor', or is not object or array. Defaulting to { 0, 180, 255, 255 }.", id.c_str());
        }

        if (sliderJSON.contains("handleColor") && (sliderJSON["handleColor"].is_array() || sliderJSON["handleColor"].is_object()))
        {
            handleColor = JSONParser::ParseJSONColor(sliderJSON["handleColor"], handleColor);
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] does not contain 'handleColor', or is not object or array. Defaulting to { 240, 240, 240, 255 }.", id.c_str());
        }

        if (sliderJSON.contains("sliderSize") && (sliderJSON["sliderSize"].is_array() || sliderJSON["sliderSize"].is_object() || sliderJSON["sliderSize"].is_number()))
        {
            sliderSize = JSONParser::ParseJSONSize<FSize>(sliderJSON["sliderSize"], sliderSize);
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] does not contain 'sliderSize', or is not object, array or number. Defaulting to { 120.0f, 4.0f }.", id.c_str());
        }

        if (sliderJSON.contains("handleSize") && (sliderJSON["handleSize"].is_array() || sliderJSON["handleSize"].is_object() || sliderJSON["handleSize"].is_number()))
        {
            handleSize = JSONParser::ParseJSONSize<FSize>(sliderJSON["handleSize"], handleSize);
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] does not contain 'handleSize', or is not object, array or number. Defaulting to { 1.0f, 16.0f }.", id.c_str());
        }

        if (!sliderJSON.contains("sliderPadding") || !sliderJSON["sliderPadding"].is_number())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ParseTextField: Text field JSON for [%s] does not contain 'sliderPadding', or is not number. Defaulting to 10.0f.", id.c_str());
        }

        const float sliderPadding = sliderJSON.value("sliderPadding", 10.0f);

        textField->SetSliderTrackColor(sliderTrackColor);
        textField->SetSliderActiveColor(sliderActiveColor);
        textField->SetHandleColor(handleColor);
        textField->SetSliderPadding(sliderPadding);
        textField->SetSliderSize(sliderSize);
        textField->SetHandleSize(handleSize);
    }

    return textField;
}
