#pragma once

#ifndef GUARDMOVETOLASTTILESTRATEGY_H_
#define GUARDMOVETOLASTTILESTRATEGY_H_

#include <SDL3/SDL_rect.h>

#include "GuardPathStrategy.h"
#include "GuardSearchStrategyProgressTracker.h"

/**
* @brief Strategy for the guard to move to the last tile the player was seen at.
* 
* Has pathfinding.
* 
* Non-shareable.
*/
class GuardMoveToLastTileStrategy : public GuardPathStrategy, public GuardSearchStrategyProgressTracker
{
public:

	/**
	* @brief Constructor.
	* 
	* @param targetX The X coordinate of the target (tile index).
	* @param targetY The Y coordinate of the target (tile index).
	* @param strategy Pathfinding strategy to be used.
	*/
	GuardMoveToLastTileStrategy(int targetX, int targetY, PathStrategy strategy = PathStrategy::AStar);

	/**
	* @brief Constructor.
	*
	* @param target The target point (tile index).
	* @param strategy Pathfinding strategy to be used.
	*/
	explicit GuardMoveToLastTileStrategy(const SDL_Point& target, PathStrategy strategy = PathStrategy::AStar);

	/**
	* @brief Default destructor.
	*/
	~GuardMoveToLastTileStrategy() override = default;

	/**
	* @brief Returns the next direction for the guard to take.
	*
	* @param guard Reference to the guard.
	*
	* @return A direction for the guard to take, or Direction::ENUM_SENTINEL_VALUE if the guard should not move, or is stuck.
	*/
	Direction GetDirection(Guard& guard) override;

	/**
	* @brief Returns the status of the search strategy.
	*
	* @return 'true' if the strategy has finished, 'false' if otherwise.
	*/
	bool IsSearchStrategyDone() const override;
};

#endif // GUARDMOVETOLASTTILESTRATEGY_H_
