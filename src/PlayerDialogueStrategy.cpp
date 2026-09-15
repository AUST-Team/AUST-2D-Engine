#include "PlayerDialogueStrategy.h"

#include "DialogueSystem.h"
#include "Player.h"
#include "PlayerStrategyManager.h"
#include "InteractCommand.h"
#include "MoveCommand.h"

void PlayerDialogueStrategy::Update(double deltaTime)
{
	DialogueSystem& dialogueSystem = DialogueSystem::GetInstance();
	dialogueSystem.Update(deltaTime);

	if (!dialogueSystem.IsActive())
	{
		PlayerStrategyManager::SetStrategy(nullptr);
	}
}

void PlayerDialogueStrategy::Setup()
{
	Player::GetInstance()->SetActiveMovement(false);
}

void PlayerDialogueStrategy::HandleCommand(const Command* command)
{
	DialogueSystem::GetInstance().HandleCommand(command);
}

PlayerStrategyType PlayerDialogueStrategy::GetStrategyType() const { return PlayerStrategyType::Dialogue; }
