#include "DialogueNarrationInteractable.h"

#include "DialogueSystem.h"
#include "PlayerStrategyManager.h"
#include "PlayerDialogueStrategy.h"

DialogueNarrationInteractable::DialogueNarrationInteractable(const std::vector<std::string>& dialogueLines, const std::function<void()>& callback) :
	lines(dialogueLines),
	callback(callback) {}

void DialogueNarrationInteractable::Interact()
{
	PlayerStrategyManager::SetStrategy(&PlayerDialogueStrategy::GetInstance());
	DialogueSystem::GetInstance().StartDialogue(lines, callback);
}

void DialogueNarrationInteractable::SetLines(const std::vector<std::string>& newLines) { lines = newLines; }

void DialogueNarrationInteractable::SetCallback(const std::function<void()>& newCallback) { callback = newCallback; }



