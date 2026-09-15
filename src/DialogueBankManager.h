#pragma once

#ifndef DIALOGUEBANK_H_
#define DIALOGUEBANK_H_

#include <vector>
#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>

#include "ResolvedChoice.h"

struct DialogueData;

/**
* @brief Static class for loading dialogue lines / nodes.
*/
class DialogueBankManager
{
private:
	// No instancing due to static class.
	DialogueBankManager() = delete;
	~DialogueBankManager() = delete;

	// No copying or moving due to static class.
	DialogueBankManager(const DialogueBankManager&) = delete;
	DialogueBankManager& operator=(const DialogueBankManager&) = delete;
	DialogueBankManager(DialogueBankManager&&) = delete;
	DialogueBankManager& operator=(DialogueBankManager&&) = delete;

	/**
	* @brief Loads a dialogue from a file.
	* 
	* @param filePath Path to the file.
	* @param count Number of lines to get. If -1, will get all lines. If count is bigger than the lines, will return all lines parsed.
	* @param randomize Flag if the lines should be in random order or not.
	* 
	* @return A vector of strings representing the parsed lines.
	*/
	static std::vector<std::string> LoadDialogueFromFile(const std::filesystem::path& filePath, int count, bool randomize = false);

	/**
	* @brief Fetches the lines from a given JSON.
	*
	* @param sourceJSON JSON containing the dialogue source; can be inlined or from a file.
	*
	* @return A vector of strings representing the parsed lines.
	*/
	static std::vector<std::string> GetLines(const nlohmann::json& sourceJSON);

	/**
	* @brief Parses and expands the choices from a JSON.
	*
	* @param choicesJSON JSON containing the choice data.
	* @param randomizeAllChoices Flag if all choices should be randomised after being parsed and expanded.
	*
	* @return A vector of ResolvedChoice
	*/
	static std::vector<ResolvedChoice> ParseAndExpandChoices(const nlohmann::json& choicesJson, bool randomizeAllChoices = false);

public:

	/**
	* @brief Executes dialogue data.
	* 
	* @param data The dialogue data.
	*/
	static void ExecuteDialogueData(const DialogueData& data);

	/**
	* @brief Executes dialogue data from a JSON.
	*
	* @param dataJSON The dialogue data in JSON form.
	*/
	static void ExecuteDialogueData(const nlohmann::json& dataJSON);

	/**
	* @brief Executes dialogue data from a node.
	*
	* @param nodeName The node's name.
	*/
	static void ExecuteDialogueData(const std::string& nodeName);
};

#endif // DIALOGUEBANK_H_
