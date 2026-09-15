#include <SDL3/SDL.h>

#include "GuardIdleChangeState.h"

#include "Guard.h"
#include "GuardStateManager.h"
#include "GuardChaseState.h"
#include "Configuration.h"
#include "MemoryTracker.h"

GuardIdleChangeState::GuardIdleChangeState(GuardState* nextState, float idleDurationTimer) :
	nextState(nextState),
	idleTimer(idleDurationTimer) {}

GuardIdleChangeState::GuardIdleChangeState(GuardIdleChangeState&& other) noexcept :
	idleTimer(other.idleTimer),
	nextState(other.nextState) { other.nextState = nullptr; }

GuardIdleChangeState::~GuardIdleChangeState()
{
	if (nextState && !nextState->IsShared())
	{
		ENG_DELETE(nextState);
		nextState = nullptr;
	}
}

void GuardIdleChangeState::Enter(Guard& guard)
{
	if (!nextState)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GuardIdleChangeState.Enter: Next state is null, defaulting to default behavior state.");
		GuardStateManager::ChangeToDefaultState(guard);
		return;
	}

	guard.StartAggressionDecrease();
	guard.StartVisionDecrease();

	guard.SetDirectionProvider(nullptr); // No direction, stay in place.
}

void GuardIdleChangeState::Update(double deltaTime, Guard& guard)
{
	if (guard.CanSeePlayer())
	{
		GuardStateManager::SetState(guard, &GuardChaseState::GetInstance());
	}
	else
	{
		idleTimer -= static_cast<float>(deltaTime);
		if (idleTimer <= 0 && nextState)
		{
			GuardStateManager::SetState(guard, nextState);
		}
	}
}

void GuardIdleChangeState::Exit(Guard& guard) { GuardState::Exit(guard); }

bool GuardIdleChangeState::IsShared() const { return false; }

GuardStateType GuardIdleChangeState::GetStateType() const { return GuardStateType::Idle; }

GuardIdleChangeState& GuardIdleChangeState::operator=(GuardIdleChangeState&& other) noexcept
{
	if (this == &other)
	{
		return *this;
	}

	if (nextState && !nextState->IsShared())
	{
		ENG_DELETE(nextState);
	}

	idleTimer = other.idleTimer;
	nextState = other.nextState;

	other.nextState = nullptr;

	return *this;
}
