#include <SDL3/SDL.h>

#include "GuardChaseState.h"

#include "Guard.h"
#include "Configuration.h"
#include "Miscs.h"
#include "Player.h"
#include "GuardStateManager.h"
#include "GuardSearchState.h"
#include "PlayerStrategyManager.h"
#include "PlayerCaughtStrategy.h"
#include "Distances.h"
#include "FloatUtils.h"
#include "MemoryTracker.h"
#include "GuardCooldownState.h"

void GuardChaseState::Enter(Guard& guard)
{
	guard.SetChaseStartTime(SDL_GetTicks()); 
	guard.SetChaseAlertFlag(true);

	const GameConfiguration& config = Configuration::Get();
	VisionRange chaseVision = config.guard.MakeVisionRange(config.guard.vision.chase);
	float chaseSpeed = config.guard.chaseSpeed;

	guard.SetVisionRange(chaseVision);
	guard.SetSpeed(chaseSpeed);
	guard.SetTimeSinceLastPlayerSeen(0.0f);
	guard.SetDirectionProvider(MakeDirectionProviderFrom(&chaseStrategy, guard));
}

void GuardChaseState::Update(double deltaTime, Guard& guard)
{
	const Player* player = Player::GetInstance();

	float distance = Distances::GetEuclidianDistanceTo(guard.GetX(), guard.GetY(), player->GetX(), player->GetY());
	guard.AddAggressionBasedOnDistance(distance, deltaTime);

	if (guard.CanShowChaseAlert())
	{
		uint64_t now = SDL_GetTicks();
		if (now - guard.GetChaseStartTime() > 200)
		{
			guard.ShowChaseAlertIcon();
			guard.SetChaseAlertFlag(false);
		}
	}

	if (HasCaughtPlayer(guard))
	{
		if (PlayerStrategyManager::GetCurrentStrategyType() != PlayerStrategyType::Caught)
		{
			PlayerCaughtStrategy::GetInstance().SetGuard(&guard);
			PlayerStrategyManager::SetStrategy(&PlayerCaughtStrategy::GetInstance());
			GuardStateManager::SetState(guard, ENG_NEW(GuardCooldownState, -1.0f));
		}
		return;
	}

	if (!guard.CanSeePlayer())
	{
		guard.AddTimeSinceLastPlayerSeen(static_cast<float>(deltaTime));
		if (guard.GetTimeSinceLastPlayerSeen() > 0.3f)
		{
			GuardStateManager::SetState(guard, ENG_NEW(GuardSearchState, player->GetTileX(), player->GetTileY(), player->GetLastDirection(), guard.GetAggressionLevel()));
			return;
		}
	}
	else
	{
		guard.SetTimeSinceLastPlayerSeen(0.0f);
	}

	chaseStrategy.Update(deltaTime);
}

void GuardChaseState::Exit(Guard& guard) { GuardState::Exit(guard); }

bool GuardChaseState::IsShared() const { return true; }

GuardStateType GuardChaseState::GetStateType() const { return GuardStateType::Alert; }

bool GuardChaseState::HasCaughtPlayer(Guard& guard)
{
	const Player* player = Player::GetInstance();

	float dx = std::abs(player->GetX() - guard.GetX());
	float dy = std::abs(player->GetY() - guard.GetY());

	const GameConfiguration& config = Configuration::Get();
	float tileWidth = static_cast<float>(config.tiles.width);
	float tileHeight = static_cast<float>(config.tiles.height);

	return ((dx == 0 && FloatUtils::IsEqualF(dy, tileHeight)) || (dy == 0 && FloatUtils::IsEqualF(dx, tileWidth)));
}
