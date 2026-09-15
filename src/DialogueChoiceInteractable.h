#pragma once

#ifndef DIALOGUECHOICEINTERACTABLE_H_
#define DIALOGUECHOICEINTERACTABLE_H_

#include <vector>
#include <string>
#include <functional>

#include "Interactable.h"

/**
* @brief Interactable for a choice dialogue.
*/
class DialogueChoiceInteractable : public Interactable
{
private:
	std::vector<std::string> choiceLines;	/// Choice lines.
	std::string questionLine;				/// Question line.
	std::vector<std::function<void()>> onChoiceCallbacks;	/// Functions (per-choice) to call once a choice has been selected.
	std::function<void()> onDialogueEndCallback;	/// Function to call once the dialogue has ended.

public:

	/**
	* @brief Constructor.
	*
	* @param choices The choices that can be selected.
	* @param onChoiceCallbacks Functions (per-choice) to call once a choice has been selected.
	* @param question Question to ask / dialogue.
	* @param onEndCallback Function to call once the dialogue has ended.
	*/
	explicit DialogueChoiceInteractable(
		const std::vector<std::string>& choices = {},
		const std::vector<std::function<void()>>& onChoiceCallbacks = {},
		const std::string& question = " ",
		const std::function<void()>& onEndCallback = nullptr
	);

	/**
	* @brief Default destructor.
	*/
	~DialogueChoiceInteractable() override = default;

	/**
	* @brief Stars the dialogue.
	*/
	void Interact() override;

	/**
	* @brief Sets the choices.
	* 
	* @param newChoice New choices for the dialogue.
	*/
	void SetChoices(const std::vector<std::string>& newChoices);

	/**
	* @brief Sets the question.
	* 
	* @param newQuestion New question for the dialogue.
	*/
	void SetQuestion(const std::string& newQuestion);

	/**
	* @brief Sets the choice callbacks.
	* 
	* @param newCallbacks New callbacks for the choice.
	*/
	void SetCallbacks(const std::vector<std::function<void()>>& newCallbacks);

	/**
	* @brief Sets the dialogue end callback.
	* 
	* @param newCallback New callback for the end of the dialogue.
	*/
	void SetOnDialogueEndCallback(const std::function<void()>& newCallback);
};
#endif // DIALOGUECHOICEINTERACTABLE_H_