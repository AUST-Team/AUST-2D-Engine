#pragma once

#ifndef GUARDFOLLOWDIRECTIONSTRATEGY_H_
#define GUARDFOLLOWDIRECTIONSTRATEGY_H_

#include "GuardStrategy.h"
#include "GuardSearchStrategyProgressTracker.h"

/**
* @brief Strategy for the guard to follow the last direction given (usually the player's).
* 
* No pathfinding.
* 
* Non-shareable.
*/
class GuardFollowDirectionStrategy : public GuardStrategy, public GuardSearchStrategyProgressTracker
{
private:
	Direction direction;		/// The direction to follow.
	int tilesLeft = 5;			/// The number of tiles to go in the respective direction.

public:

	/**
	* @brief Constructor.
	* 
	* @param dir The direction to take.
	* @param tiles The number of tiles to walk.
	*/
	explicit GuardFollowDirectionStrategy(Direction dir, int tiles = 5);

	/**
	* @brief Default destructor.
	*/
	~GuardFollowDirectionStrategy() override = default;

	/**
	* @brief Returns the next direction for the guard to take.
	*
	* @param guard Reference to the guard.
	*
	* @return A direction for the guard to take, or Direction::ENUM_SENTINEL_VALUE if the guard should not move, or is stuck.
	*/
	Direction GetDirection(Guard& guard) override;

	/**
	* @brief Does nothing as this strategy doesn't rely on delta time.
	*
	* @param delteTime The delta time of the main game loop.
	*/
	void Update(double deltaTime) override;

	/**
	* @brief Returns the status of the search strategy.
	*
	* @return 'true' if the strategy has finished, 'false' if otherwise.
	*/
	bool IsSearchStrategyDone() const override;
};

#endif // GUARDFOLLOWDIRECTIONSTRATEGY_H_