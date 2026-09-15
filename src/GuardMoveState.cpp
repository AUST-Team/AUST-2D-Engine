#include <SDL3/SDL.h>

#include "GuardMoveState.h"

#include "Guard.h"
#include "GuardStateManager.h"
#include "GuardFallbackState.h"
#include "Miscs.h"
#include "GuardPathStrategy.h"
#include "Configuration.h"
#include "GuardChaseState.h"
#include "Unused.h"

void GuardMoveState::Enter(Guard& guard)
{
	if (!guard.GetAIData().data.moveToPointStrategy)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GuardMoveState.Enter: Null move to point strategy for guard. Defaulting to fallback state.");
		GuardStateManager::SetState(guard, &GuardFallbackState::GetInstance());
		return;
	}

	if (guard.GetAIData().type != GuardBehaviorType::IdleReturn)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GuardMoveState.Enter: Guard is in incorrect behavior mode for idle and return. Defaulting to fallback state.");
		GuardStateManager::SetState(guard, &GuardFallbackState::GetInstance());
		return;
	}

	guard.StartAggressionDecrease();
	guard.StartVisionDecrease();

	guard.SetSpeed(Configuration::Get().guard.patrolSpeed);
	guard.SetDirectionProvider(MakeDirectionProviderFrom(guard.GetAIData().data.moveToPointStrategy, guard));
	guard.GetAIData().data.moveToPointStrategy->ForcePathReconstruction();
}

void GuardMoveState::Update(double deltaTime, Guard& guard)
{
	if (guard.CanSeePlayer())
	{
		//GuardStateManager::SetState(guard, &GuardChaseState::GetInstance());
	}
	else if (guard.GetAIData().data.moveToPointStrategy->HasReachedPoint())
	{
		guard.GetAIData().data.moveToPointStrategy->ResetReachedPointFlag();
		// A little hack to re-use the fallback state for the 'idle' behavior.
		GuardStateManager::SetState(guard, &GuardFallbackState::GetInstance());
	}

	guard.GetAIData().data.moveToPointStrategy->Update(deltaTime);
}

void GuardMoveState::Exit(Guard& guard) { GuardState::Exit(guard); }

bool GuardMoveState::IsShared() const { return true; }

GuardStateType GuardMoveState::GetStateType() const { return GuardStateType::Moving; }
