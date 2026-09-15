#include "DialogueChoiceInteractable.h"

#include "PlayerStrategyManager.h"
#include "PlayerDialogueStrategy.h"
#include "DialogueSystem.h"

DialogueChoiceInteractable::DialogueChoiceInteractable(const std::vector<std::string>& choices, const std::vector<std::function<void()>>& onChoiceCallbacks, const std::string& question, const std::function<void()>& onEndCallback) :
	choiceLines(choices),
	onChoiceCallbacks(onChoiceCallbacks),
	questionLine(question),
	onDialogueEndCallback(onEndCallback){}

void DialogueChoiceInteractable::Interact()
{
	PlayerStrategyManager::SetStrategy(&PlayerDialogueStrategy::GetInstance());
	DialogueSystem::GetInstance().StartChoice(choiceLines, onChoiceCallbacks, questionLine, onDialogueEndCallback);
}

void DialogueChoiceInteractable::SetChoices(const std::vector<std::string>& newChoices) { choiceLines = newChoices; }

void DialogueChoiceInteractable::SetQuestion(const std::string& newQuestion) { questionLine = newQuestion; }

void DialogueChoiceInteractable::SetCallbacks(const std::vector<std::function<void()>>& newCallbacks) { onChoiceCallbacks = newCallbacks; }

void DialogueChoiceInteractable::SetOnDialogueEndCallback(const std::function<void()>& newCallback) { onDialogueEndCallback = newCallback; }


