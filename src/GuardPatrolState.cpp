#include <SDL3/SDL.h>

#include "GuardPatrolState.h"

#include "Guard.h"
#include "GuardStateManager.h"
#include "GuardFallbackState.h"
#include "Configuration.h"
#include "Miscs.h"
#include "GuardPatrolStrategy.h"
#include "GuardIdleChangeState.h"
#include "GuardChaseState.h"
#include "MemoryTracker.h"
#include "Unused.h"

void GuardPatrolState::Enter(Guard& guard)
{
	if (!guard.GetAIData().data.patrolStrategy)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GuardPatrolState.Enter: Null patrol strategy for guard. Defaulting to fallback state.");
		GuardStateManager::SetState(guard, &GuardFallbackState::GetInstance());
		return;
	}

	if (guard.GetAIData().type != GuardBehaviorType::Patrol)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GuardPatrolState.Enter: Guard is in incorrect behavior mode for patrol. Defaulting to fallback state.");
		GuardStateManager::SetState(guard, &GuardFallbackState::GetInstance());
		return;
	}

	guard.StartAggressionDecrease();
	guard.StartVisionDecrease();

	guard.SetSpeed(Configuration::Get().guard.patrolSpeed);
	guard.SetDirectionProvider(MakeDirectionProviderFrom(guard.GetAIData().data.patrolStrategy, guard));
}

void GuardPatrolState::Update(double deltaTime, Guard& guard)
{
	if (guard.CanSeePlayer())
	{
		//GuardStateManager::SetState(guard, &GuardChaseState::GetInstance());
	}
	else if (guard.GetAIData().data.patrolStrategy->HasReachedPatrolPoint())
	{
		GuardIdleChangeState* pauseState = ENG_NEW(GuardIdleChangeState, &GetInstance(), Configuration::Get().guard.timings.patrolPause);
		GuardStateManager::SetState(guard, pauseState);
	}

	guard.GetAIData().data.patrolStrategy->Update(deltaTime);
}

void GuardPatrolState::Exit(Guard& guard) { GuardState::Exit(guard); }

bool GuardPatrolState::IsShared() const { return true; }

GuardStateType GuardPatrolState::GetStateType() const { return GuardStateType::Moving; }
