#pragma once

#ifndef DIALOGUENARRATIONINTERACTABLE_H_
#define DIALOGUENARRATIONINTERACTABLE_H_

#include <vector>
#include <string>
#include <functional>

#include "Interactable.h"

/**
* @brief Interactable for a narration dialogue.
*/
class DialogueNarrationInteractable : public Interactable
{
private:
	std::vector<std::string> lines;				/// Dialogue lines.
	std::function<void()> callback = nullptr;	/// Function to call on dialogue end

public:

	/**
	* @brief Constructor.
	* 
	* @param dialogueLines The lines of dialogue.
	* @param callback Function to call on dialogue end.
	*/
	explicit DialogueNarrationInteractable(const std::vector<std::string>& dialogueLines = {}, const std::function<void()>& callback = nullptr);

	/**
	* @brief Default destructor.
	*/
	~DialogueNarrationInteractable() override = default;

	/**
	* @brief Stars the dialogue.
	*/
	void Interact() override;

	/**
	* @brief Sets the lines of dialogue.
	* 
	* @param newLines New lines of dialogue.
	*/
	void SetLines(const std::vector<std::string>& newLines);

	/**
	* @brief Sets the callback at the end of dialogue.
	* 
	* @param newCallback New callback for the end of dialogue.
	*/
	void SetCallback(const std::function<void()>& newCallback);
};

#endif // DIALOGUENARRATIONINTERACTABLE_H_