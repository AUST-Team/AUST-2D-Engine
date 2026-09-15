#include "PlayerStrategyManager.h"

#include "DialogueSystem.h"
#include "Player.h"
#include "PlayerStrategy.h"
#include "Command.h"

/**
* @brief Sets the strategy to the normal strategy.
* 
* Declared and defined here for internal linkage only.
*/
static void SetNormal()
{
    DialogueSystem::GetInstance().EndDialogue();
    Player::GetInstance()->SetActiveMovement(true);
    Player::GetInstance()->SetCurrentStrategy(nullptr);
}

void PlayerStrategyManager::SetStrategy(PlayerStrategy* newStrategy)
{
    if (!newStrategy) 
    {
        SetNormal();
        return;
    }

	newStrategy->Setup();
    Player::GetInstance()->SetCurrentStrategy(newStrategy);
}

void PlayerStrategyManager::Update(double deltaTime)
{
    Player* player = Player::GetInstance();

    if (player->GetCurrentStrategy())
    {
        player->GetCurrentStrategy()->Update(deltaTime);
    }
}

void PlayerStrategyManager::HandleCommand(Command* command)
{
    Player* player = Player::GetInstance();

    if (player->GetCurrentStrategy())
    {
        player->GetCurrentStrategy()->HandleCommand(command);
    }

    // Normal strategy command handling is done directly from the Command class.
    // I am still unsure if I should do it here or keep it in the Command class.
}

PlayerStrategyType PlayerStrategyManager::GetCurrentStrategyType()
{
    Player* player = Player::GetInstance();

    if (player->GetCurrentStrategy())
    {
        return player->GetCurrentStrategy()->GetStrategyType();
    }
    
    return PlayerStrategyType::Normal;
}
