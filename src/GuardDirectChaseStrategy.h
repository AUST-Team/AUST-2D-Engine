#pragma once

#ifndef GUARDDIRECTCHASESTRATEGY_H_
#define GUARDDIRECTCHASESTRATEGY_H_

#include "GuardStrategy.h"

/**
* @brief Direct chase strategy for the guards.
* 
* No pathfinding. Follows on the player, taking the shortest possible path.
* 
* Shareable.
*/
class GuardDirectChaseStrategy : public GuardStrategy
{
public:

	/**
	* @brief Default destructor.
	*/
	~GuardDirectChaseStrategy() override = default;

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
};

#endif // GUARDDIRECTCHASESTRATEGY_H_