#include <random>
#include <SDL3/SDL.h>

#include "PlayerCaughtStrategy.h"

#include "DialogueSystem.h"
#include "Player.h"
#include "GuardStateManager.h"
#include "GuardCooldownState.h"
#include "PlayerStrategyManager.h"
#include "AppStateManager.h"
#include "CutsceneState.h"
#include "Configuration.h"
#include "MemoryTracker.h"
#include "DataProvider.h"
#include "Guard.h"
#include "DialogueBankManager.h"
#include "GameMap.h"

void PlayerCaughtStrategy::Update(double deltaTime)
{
    if (!guard)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "PlayerCaughtStrategy.Update: Guard pointer is null. Defaulting to normal strategy.");
        PlayerStrategyManager::SetStrategy(nullptr);
    }

    DialogueSystem& dialogueSystem = DialogueSystem::GetInstance();
    dialogueSystem.Update(deltaTime);

    if (!dialogueSystem.IsActive())
    {
        PlayerStrategyManager::SetStrategy(nullptr);
    }
}

void PlayerCaughtStrategy::Setup()
{
    if (!guard)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "PlayerCaughtStrategy.Setup: Guard pointer is null. Defaulting to normal strategy.");
        PlayerStrategyManager::SetStrategy(nullptr);
        return;
    }

    const std::optional<int> caughtCount = DataProvider::GetInstance().FetchData<int>(ConstantConfiguration::playerCaughtDataKey);

    if (!caughtCount || *caughtCount < 0)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "PlayerCaughtStrategy.Setup: Key [%s] returned negative/missing data.", ConstantConfiguration::playerCaughtDataKey);
        PlayerStrategyManager::SetStrategy(nullptr);
        return;
	}

    Player::GetInstance()->IncrementTimesCaught();

    if (guard->GetCaughtDialogueData())
    {
        DialogueBankManager::ExecuteDialogueData(*guard->GetCaughtDialogueData());
    }
    else
    {
        DialogueBankManager::ExecuteDialogueData(GameMap::GetInstance().GetDialogueData());
    }
}

void PlayerCaughtStrategy::HandleCommand(const Command* command) { DialogueSystem::GetInstance().HandleCommand(command); }

PlayerStrategyType PlayerCaughtStrategy::GetStrategyType() const { return PlayerStrategyType::Caught; }

void PlayerCaughtStrategy::SetGuard(Guard* newGuard) { guard = newGuard; }

Guard* PlayerCaughtStrategy::GetGuard() const { return guard; }
