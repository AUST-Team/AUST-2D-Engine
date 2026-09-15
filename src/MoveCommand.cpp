#include "MoveCommand.h"

#include "Player.h"
#include "AppStateManager.h"
#include "PlayerStrategyManager.h"
#include "PlayerStrategy.h"

MoveCommand::MoveCommand(Direction direction) : 
	direction(direction) {}

void MoveCommand::Execute()
{
	if (AppStateManager::GetInstance().IsCurrentStatePause())
	{
		return;
	}

	switch (PlayerStrategyManager::GetCurrentStrategyType())
	{
		case PlayerStrategyType::Normal:
		{
			Player::GetInstance()->TryMove(direction);
			break;
		}

		default:
		{
			Player::GetInstance()->GetCurrentStrategy()->HandleCommand(this);
			break;
		}
	}
}

CommandType MoveCommand::GetType() const { return CommandType::Movement; }

Direction MoveCommand::GetDirection() const { return direction; }
