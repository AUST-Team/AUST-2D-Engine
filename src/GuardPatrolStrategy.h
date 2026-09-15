#pragma once

#ifndef GUARDPATROLSTRATEGY_H_
#define GUARDPATROLSTRATEGY_H_

#include "GuardPathStrategy.h"

/**
* @brief Strategy for a guard to patrol a set number of points. 
* 
* Has pathfinding.
* 
* Non-shareable.
*/
class GuardPatrolStrategy : public GuardPathStrategy
{
private:
	std::vector<SDL_Point> patrolPoints;	/// Vector of patrol points. They should be the index of tiles.
	size_t currentPatrolIndex = 0;				/// The current point the guard is at.
	bool reachedPatrolPointFlag = true;			/// Flag if the guard reached a point or not.
	bool setNewTargetFlag = true;				/// Flag if a new target (patrol point) should be set.

public:

	/**
	* @brief Constructor.
	* 
	* @param points The patrol points (index of tiles) the guard will take.
	*/
	explicit GuardPatrolStrategy(const std::vector<SDL_Point>& points);

	/**
	* @brief Default destructor.
	*/
	~GuardPatrolStrategy() override = default;

	/**
	* @brief Returns the next direction for the guard to take.
	*
	* @param guard Reference to the guard.
	*
	* @return A direction for the guard to take, or Direction::ENUM_SENTINEL_VALUE if the guard should not move, or is stuck.
	*/
	Direction GetDirection(Guard& guard) override;

	/**
	* @brief Returns if the guard has reached a point or not.
	* 
	* @return 'true' if the guard has reached a patrol point, 'false' if not.
	*/
	bool HasReachedPatrolPoint() const;
};

#endif	// GUARDPATROLSTRATEGY_H_