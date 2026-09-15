#include "GuardCooldownState.h"

#include "Guard.h"
#include "Configuration.h"
#include "GuardStateManager.h"

GuardCooldownState::GuardCooldownState(float cooldownDurationTimer) :
	cooldownTimer(cooldownDurationTimer) {}

void GuardCooldownState::Enter(Guard& guard)
{
	const GameConfiguration& config = Configuration::Get();
	const VisionRangeModifierConfiguration& patrolMod = config.guard.vision.patrol;
	VisionRange patrolVision = config.guard.MakeVisionRange(patrolMod);

	guard.SetAggression(0.0f);
	guard.SetVisionRange(patrolVision);
	guard.SetDirectionProvider(nullptr);
}

void GuardCooldownState::Update(double deltaTime, Guard& guard)
{
	if (cooldownTimer == -1.0f)
	{
		return;
	}

	cooldownTimer -= static_cast<float>(deltaTime);
	if (cooldownTimer <= 0.0f)
	{
		GuardStateManager::ChangeToDefaultState(guard);
	}
}

void GuardCooldownState::Exit(Guard& guard) { GuardState::Exit(guard); }

bool GuardCooldownState::IsShared() const { return false; }

GuardStateType GuardCooldownState::GetStateType() const { return GuardStateType::Alert; }
