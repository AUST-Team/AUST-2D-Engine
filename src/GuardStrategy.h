#pragma once

#ifndef GUARDSTRATEGY_H_
#define GUARDSTRATEGY_H_

#include <optional>

#include "Direction.h"

class Guard;

/**
* @brief Abstract class for a guard strategy.
* 
* Strategies can be shared if they don't contain any data (usually).
*/
class GuardStrategy
{
public:

	/**
	* @brief Default destructor.
	*/
	virtual ~GuardStrategy() = default;

	/**
	* @brief Updates the strategy.
	* 
	* @param delteTime The delta time of the main game loop.
	*/
	virtual void Update(double deltaTime) = 0;

	/**
	* @brief Returns the next direction for the guard to take.
	* 
	* @param guard Reference to the guard.
	* 
	* @return A direction for the guard to take, or Direction::ENUM_SENTINEL_VALUE if the guard should not move, or is stuck.
	*/
	virtual Direction GetDirection(Guard& guard) = 0;
};

#endif // GUARDSTRATEGY_H_