#include "GuardFollowDirectionStrategy.h"

#include "Guard.h"

GuardFollowDirectionStrategy::GuardFollowDirectionStrategy(Direction dir, int tiles) :
	direction(dir),
	tilesLeft(tiles) {}

Direction GuardFollowDirectionStrategy::GetDirection(Guard& guard) 
{ 
	if (IsSearchStrategyDone())
	{
		return GetSentinel<Direction>();
	}

	if (!guard.IsMoving())
	{
		--tilesLeft;
	}

	return direction; 
}

void GuardFollowDirectionStrategy::Update(double deltaTime) { GuardStrategy::Update(deltaTime); }

bool GuardFollowDirectionStrategy::IsSearchStrategyDone() const { return tilesLeft <= 0; }
