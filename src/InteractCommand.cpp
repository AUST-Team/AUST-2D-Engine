#include "InteractCommand.h"

#include "Player.h"
#include "PlayerStrategyManager.h"
#include "PlayerStrategy.h"
#include "DialogueSystem.h"
#include "GameMap.h"

void InteractCommand::Execute()
{
	switch (PlayerStrategyManager::GetCurrentStrategyType())
	{
		case PlayerStrategyType::Normal:
		{
			GameMap::GetInstance().RequestInteract();
			break;
		}

		default:
		{
			Player::GetInstance()->GetCurrentStrategy()->HandleCommand(this);
			break;
		}
	}
}

CommandType InteractCommand::GetType() const { return CommandType::Action; }
