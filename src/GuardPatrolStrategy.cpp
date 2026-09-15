#include <SDL3/SDL.h>

#include "GuardPatrolStrategy.h"

#include "GuardStateManager.h"
#include "GuardFallbackState.h"
#include "Guard.h"
#include "Direction.h"
#include "FloatUtils.h"

GuardPatrolStrategy::GuardPatrolStrategy(const std::vector<SDL_Point>& points) :
	patrolPoints(points) {}

Direction GuardPatrolStrategy::GetDirection(Guard& guard)
{
	if (patrolPoints.empty())
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GuardPatrolStrategy.GetDirection: Patrol points are empty, defaulting to fallback state.");
		GuardStateManager::SetState(guard, &GuardFallbackState::GetInstance());
		return GetSentinel<Direction>();
	}

	if (setNewTargetFlag)
	{
		SetTarget(patrolPoints[currentPatrolIndex]);
		setNewTargetFlag = false;
		reachedPatrolPointFlag = false;
	}

	if (reachedPointFlag)
	{
		currentPatrolIndex = (currentPatrolIndex + 1) % patrolPoints.size();
		reachedPatrolPointFlag = true;
		setNewTargetFlag = true;
		return GetSentinel<Direction>();
	}

	return GuardPathStrategy::GetDirection(guard);
}

bool GuardPatrolStrategy::HasReachedPatrolPoint() const { return reachedPatrolPointFlag; }
