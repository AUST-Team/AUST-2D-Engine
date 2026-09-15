#pragma once

#ifndef PLAYERSTRATEGYMANAGER_H_
#define PLAYERSTRATEGYMANAGER_H_

#include "PlayerStrategyType.h"

class PlayerStrategy;
class Command;

/**
* @brief Namespace that contains methods for managing the player's current strategy.
*/
namespace PlayerStrategyManager
{
	/**
	* @brief Sets the new strategy.
	*
	* It is safe to pass nullptr to this method.
	*
	* @param newStrategy The new strategy to be set, or nullptr for 'normal' strategy.
	*/
	void SetStrategy(PlayerStrategy* newStrategy);

	/**
	* @brief Updates the current strategy.
	*
	* @param deltaTime The delta time of the main SDL loop.
	*/
	void Update(double deltaTime);

	/**
	* @brief Handles a command.
	*
	* Delegates the handling to the current state.
	*
	* @param command The command to be handled.
	*/
	void HandleCommand(Command* command);

	/**
	* @brief Returns the current player's strategy type.
	*/
	PlayerStrategyType GetCurrentStrategyType();
}

#endif // PLAYERSTRATEGYMANAGER_H_
