#include "GuardMoveToLastTileStrategy.h"

#include "Guard.h"

GuardMoveToLastTileStrategy::GuardMoveToLastTileStrategy(int targetX, int targetY, PathStrategy strategy) :
	GuardPathStrategy({targetX, targetY}, strategy) {}

GuardMoveToLastTileStrategy::GuardMoveToLastTileStrategy(const SDL_Point& target, PathStrategy strategy) :
    GuardPathStrategy(target, strategy) {}

Direction GuardMoveToLastTileStrategy::GetDirection(Guard& guard)
{
    if (IsSearchStrategyDone())
    {
        return GetSentinel<Direction>();
    }

    return GuardPathStrategy::GetDirection(guard);
}

bool GuardMoveToLastTileStrategy::IsSearchStrategyDone() const { return HasReachedPoint(); }
