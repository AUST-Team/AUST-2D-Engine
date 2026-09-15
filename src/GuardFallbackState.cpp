#include "GuardFallbackState.h"

#include "Guard.h"
#include "GuardStateManager.h"
#include "GuardChaseState.h"
#include "Unused.h"

void GuardFallbackState::Enter(Guard& guard)
{
	guard.StartAggressionDecrease();
	guard.StartVisionDecrease();

	guard.SetDirectionProvider(nullptr);
}

void GuardFallbackState::Update(double deltaTime, Guard& guard)
{
	UNUSED(deltaTime);
	if (guard.CanSeePlayer())
	{
		GuardStateManager::SetState(guard, &GuardChaseState::GetInstance());
	}
}

void GuardFallbackState::Exit(Guard& guard) { GuardState::Exit(guard); }

bool GuardFallbackState::IsShared() const { return true; }

GuardStateType GuardFallbackState::GetStateType() const { return GuardStateType::Idle; }
