#include "SDL3/SDL_log.h"

#include "TimedCondition.h"

#include "GameTime.h"
#include "Trigger.h"
#include "Unused.h"

TimedCondition::TimedCondition(TimedConditionType type, uint64_t startTime, uint64_t endTime) :
	params({.type = type, .startTime = startTime, .endTime = endTime}) {}

TimedCondition::TimedCondition(const TimedConditionParams& params) :
	params(params) {}

bool TimedCondition::IsSatisfied(Trigger* trigger, Player* player, MapUpdateFlags updateMask) const
{
	UNUSED(player);
	UNUSED(updateMask);
	const uint64_t currentTime = GameTime::PeekPlayTimeMiliseconds();

	if (!isInitializedFlag)
	{
		const uint64_t triggerCreatedTime = trigger->GetCreatedTime();
		absoluteTargetTime = triggerCreatedTime + params.startTime;
		lastCheckedTime = triggerCreatedTime;
		isInitializedFlag = true;
	}

	switch (params.type)
	{
		case TimedConditionType::Before:
		{
			return currentTime < absoluteTargetTime;
		}

		case TimedConditionType::After:
		{
			return currentTime >= absoluteTargetTime;
		}

		case TimedConditionType::Specific:
		{
			const bool crossedMilestone = (lastCheckedTime < absoluteTargetTime && currentTime >= absoluteTargetTime);
			lastCheckedTime = currentTime;
			return crossedMilestone;
		}

		case TimedConditionType::Interval:
		{
			return currentTime >= absoluteTargetTime && currentTime <= (absoluteTargetTime + (params.endTime - params.startTime));
		}

		default:
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TimedCondition.IsSatisfied: Invalid timed condition type.");
			return false;
		}
	}
}

MapUpdateFlags TimedCondition::GetUpdateFlags() const { return MapUpdateFlags::None; }

uint64_t TimedCondition::GetSpatialRadius() const { return Condition::GetSpatialRadius(); }

bool TimedCondition::IsShared() const { return false; }


