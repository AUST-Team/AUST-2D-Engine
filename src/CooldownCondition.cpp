#include "CooldownCondition.h"

#include "Trigger.h"
#include "GameTime.h"
#include "Unused.h"

CooldownCondition::CooldownCondition(uint64_t cooldownTime) :
	cooldownTime(cooldownTime) {}

bool CooldownCondition::IsSatisfied(Trigger* trigger, Player* player, MapUpdateFlags updateMask) const
{
    UNUSED(player);
    UNUSED(updateMask);

    if (trigger->GetActivationCount() == 0)
    {
        return true;
    }

    const uint64_t lastActivatedTime = trigger->GetLastActivatedTime();
    const uint64_t currentTime = GameTime::PeekPlayTimeMiliseconds();

    return (currentTime - lastActivatedTime) >= cooldownTime;
}

MapUpdateFlags CooldownCondition::GetUpdateFlags() const { return MapUpdateFlags::None; }

uint64_t CooldownCondition::GetSpatialRadius() const { return Condition::GetSpatialRadius(); }

bool CooldownCondition::IsShared() const { return false; }
