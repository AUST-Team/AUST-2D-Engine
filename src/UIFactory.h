#pragma once

#ifndef UIFACTORY_H_
#define UIFACTORY_H_

#include <nlohmann/json.hpp>
#include <unordered_map>
#include <string>
#include <filesystem>

#include "UIType.h"
#include "SystemAction.h"

class UIPanel;
class UIDialogue;
class UIButton;

namespace UIFactory
{

	/**
	* @brief Creates a panel from a (JSON) file.
	*
	* @param filePath File path to the JSON file to create the panel from.
	* @param id ID of the panel.
	* @param actionCallback The callback function to be called when an action is triggered.
	*
	* @return Pointer to the created panel, or nullptr if panel cannot be created. The caller is responsible for deleting the panel.
	*/
	UIPanel* CreatePanelFromFile(const std::filesystem::path& filePath, const std::string& id, std::function<void(const SystemAction&)> actionCallback);

	/**
	* @brief Creates a panel from a JSON.
	* 
	* @param panelJSON The JSON to create the panel from.
	* @param id ID of the panel.
	* @param actionCallback The callback function to be called when an action is triggered.
	* 
	* @return Pointer to the created panel, or nullptr if panel cannot be created. The caller is responsible for deleting the panel.
	*/
	UIPanel* CreatePanelFromJSON(const nlohmann::json& panelJSON, const std::string& id, std::function<void(const SystemAction&)> actionCallback);

	/**
	* @brief Creates an UI from a (JSON) file.
	*
	* @param filePath File path to the JSON file to create the UI from.
	* @param [out] panels Map of [panelName, UIPanel*].
	* @param actionCallback The callback function to be called when an action is triggered.
	*
	* @return Pointer to the main panel, or nullptr if panel cannot be created. The caller is responsible for deleting the panels.
	*/
	UIPanel* CreateUIFromFile(const std::filesystem::path& filePath, std::unordered_map<std::string, UIPanel*>& panels, std::function<void(const SystemAction&)> actionCallback);

	/**
	* @brief Creates an UI from a JSON.
	*
	* @param uiJSON The JSON to create the UI from.
	* @param [out] panels Map of [panelName, UIPanel*].
	* @param actionCallback The callback function to be called when an action is triggered.
	*
	* @return Pointer to the main panel, or nullptr if panel cannot be created. The caller is responsible for deleting the panels.
	*/
	UIPanel* CreateUIFromJSON(const nlohmann::json& uiJSON, std::unordered_map<std::string, UIPanel*>& panels, std::function<void(const SystemAction&)> actionCallback);

	/**
	* @brief Creates an UI by type.
	* 
	* This will interrogate the configuration parameters, and create said UI.
	*
	* @param type The type of UI to create.
	* @param [out] panels Map of [panelName, UIPanel*].
	* @param actionCallback The callback function to be called when an action is triggered.
	*
	* @return Pointer to the main panel, or nullptr if panel cannot be created. The caller is responsible for deleting the panels.
	*/
	UIPanel* CreateUIByType(UIType type, std::unordered_map<std::string, UIPanel*>& panels, std::function<void(const SystemAction&)> actionCallback);

	/**
	* @brief Creates a cutscene UI from name.
	*
	* @param cutsceneName The name of the cutscene
	* @param [out] panels Map of [panelName, UIPanel*].
	* @param actionCallback The callback function to be called when an action is triggered.
	*
	* @return Pointer to the main panel, or nullptr if panel cannot be created. The caller is responsible for deleting the panels.
	*/
	UIPanel* CreateCutsceneFromName(const std::string& cutsceneName, std::unordered_map<std::string, UIPanel*>& panels, std::function<void(const SystemAction&)> actionCallback);

	/**
	* @brief Creates a dialogue UI from configuration.
	* 
	* @return Pointer to the created dialogue, or nullptr if dialogue cannot be created. The caller is responsible for deleting the dialogue.
	*/
	UIDialogue* CreateDialogue();

	/**
	* @brief Creates a dialogue UI from file.
	* 
	* @param filePath The path to the dialogue JSON file.
	* 
	* @return Pointer to the created dialogue, or nullptr if dialogue cannot be created. The caller is responsible for deleting the dialogue.
	*/
	UIDialogue* CreateDialogueFromFile(const std::filesystem::path& filePath);

	/**
	* @brief Creates a dialogue UI from a JSON.
	*
	* @param dialogueJSON JSON to create the dialogue from.
	*
	* @return Pointer to the created dialogue, or nullptr if dialogue cannot be created. The caller is responsible for deleting the dialogue.
	*/
	UIDialogue* CreateDialogueFromJSON(const nlohmann::json& dialogueJSON);

	/**
	* @brief Attaches button actions to a button.
	*
	* @param button Pointer to the button.
	* @param buttonJSON JSON containing the button data.
	* @param actionCallback The callback function to be called when an action is triggered.
	*/
	void AttachButtonActions(UIButton* button, const nlohmann::json& buttonJSON, std::function<void(const SystemAction&)> actionCallback);

	UIPanel* CreatePanelFromFile(const std::string& filePath, const std::string& id, std::function<void(const SystemAction&)> actionCallback) = delete;
	UIPanel* CreatePanelFromFile(const char * filePath, const std::string& id, std::function<void(const SystemAction&)> actionCallback) = delete;
	UIPanel* CreateUIFromFile(const std::string& filePath, std::unordered_map<std::string, UIPanel*>& panels, std::function<void(const SystemAction&)> actionCallback) = delete;
	UIPanel* CreateUIFromFile(const char * filePath, std::unordered_map<std::string, UIPanel*>& panels, std::function<void(const SystemAction&)> actionCallback) = delete;
	UIDialogue* CreateDialogueFromFile(const std::string& filePath) = delete;
	UIDialogue* CreateDialogueFromFile(const char * filePath) = delete;
}

#endif // UIFACTORY_H_