#include "MaxActivationsCondition.h"

#include "Trigger.h"
#include "Unused.h"

MaxActivationsCondition::MaxActivationsCondition(uint64_t maxActivations) :
	maxActivations(maxActivations) {}

bool MaxActivationsCondition::IsSatisfied(Trigger* trigger, Player* player, MapUpdateFlags updateMask) const
{
	UNUSED(player);
	UNUSED(updateMask);
	// Since activations start at 0, we use <
	return trigger->GetActivationCount() < maxActivations;
}

MapUpdateFlags MaxActivationsCondition::GetUpdateFlags() const { return MapUpdateFlags::None; }

uint64_t MaxActivationsCondition::GetSpatialRadius() const { return Condition::GetSpatialRadius(); }

bool MaxActivationsCondition::IsShared() const { return false; }
