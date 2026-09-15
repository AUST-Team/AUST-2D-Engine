#pragma once

#ifndef PLAYERSTRATEGY_H_
#define PLAYERSTRATEGY_H_

#include "PlayerStrategyType.h"

class Command;

/**
* @brief Abstract class for the player strategy.
*/
class PlayerStrategy
{
public:
	
	/**
	* @brief Default destructor.
	*/
	virtual ~PlayerStrategy() = default;

	/**
	* @brief Updates the strategy.
	* 
	* @param deltaTime The delta time of the main SDL loop.
	*/
	virtual void Update(double deltaTime) = 0;
	
	/**
	* @brief Sets up the strategy. Should only be executed once per strategy change.
	*/
	virtual void Setup() = 0;

	/**
	* @brief Handles a command.
	* 
	* @param command Pointer to the command.
	*/
	virtual void HandleCommand(const Command* command) = 0;

	/**
	* @brief Returns the strategy type.
	* 
	* @return The strategy type.
	*/
	virtual PlayerStrategyType GetStrategyType() const = 0;
};

#endif // PLAYERSTRATEGY_H_
