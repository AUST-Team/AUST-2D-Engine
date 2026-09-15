#include <SDL3/SDL.h>

#include "GuardStateManager.h"

#include "GuardState.h"
#include "Guard.h"
#include "GuardMoveState.h"
#include "GuardPatrolState.h"
#include "MemoryTracker.h"

#include <typeinfo>

void GuardStateManager::SetState(Guard& guard, GuardState* newState)
{
	if (guard.GetCurrentState())
	{
		guard.GetCurrentState()->Exit(guard);

		//SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GuardStateManager.SetState: Exiting from state of type: %s", typeid(*guard.GetCurrentState()).name());

		// If the state isn't shared, it must be deleted before changing.
		if (!guard.GetCurrentState()->IsShared())
		{
			ENG_DELETE(guard.GetCurrentState());
			guard.SetCurrentState(nullptr);
		}
	}

	if (!newState)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GuardStateManager.SetState: Attempted to switch to a null state. Changing to default state.");
		GuardStateManager::ChangeToDefaultState(guard);
		return;
	}

	//SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GuardStateManager.SetState: Entering to state of type: %s", typeid(*newState).name());
	guard.SetCurrentState(newState); // Don't destroy the current or new state here.

	guard.GetCurrentState()->Enter(guard);
}

void GuardStateManager::Update(double deltaTime, Guard& guard)
{
	if (!guard.GetCurrentState())
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GuardStateManager.Update: Attempted to update a null state. Changing to default state.");
		GuardStateManager::ChangeToDefaultState(guard);
		return;
	}

	guard.GetCurrentState()->Update(deltaTime, guard);
}

void GuardStateManager::ChangeToDefaultState(Guard& guard)
{
	switch (guard.GetAIData().type)
	{
		case GuardBehaviorType::IdleReturn:
		{
			GuardStateManager::SetState(guard, &GuardMoveState::GetInstance());
			break;
		}

		case GuardBehaviorType::Patrol:
		{
			GuardStateManager::SetState(guard, &GuardPatrolState::GetInstance());
			break;
		}

		default:
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GuardStateManager.ChangeToDefaultState: Guard has unknown behavior type. Defaulting to IdleReturn.");
			GuardStateManager::SetState(guard, &GuardMoveState::GetInstance());
			break;
		}
	}
}


