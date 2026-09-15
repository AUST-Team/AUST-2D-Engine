#include "DialogueState.h"

#include "Unused.h"

DialogueState::DialogueState(const std::function<void()>& onDialogueEnd) :
	onDialogueEndCallback(onDialogueEnd) {}

void DialogueState::HandleEvent(const SDL_Event& e) { UNUSED(e); }

void DialogueState::Exit(UIDialogue& dialogueRef) 
{ 
	UNUSED(dialogueRef);

	if (onDialogueEndCallback)
	{
		onDialogueEndCallback();
	}
}

void DialogueState::HandleCommand(const Command* command) { UNUSED(command); }

void DialogueState::Resize(float widthScale, float heightScale)
{
	UNUSED(widthScale);
	UNUSED(heightScale);
}

void DialogueState::RefreshState() {}

void DialogueState::SetOnDialogueEnd(const std::function<void()>& callback) { onDialogueEndCallback = callback; }

bool DialogueState::IsShared() const { return true; }