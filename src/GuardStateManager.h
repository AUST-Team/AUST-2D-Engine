#pragma once
#pragma once

#ifndef GUARDSTATEMANAGER_H_
#define GUARDSTATEMANAGER_H_

class GuardState;
class Guard;

/**
* @brief A state manager for the guards.
*/
namespace GuardStateManager
{
	/**
	* @brief Sets a new state for a guard.
	*
	* @param guard The guard of which to change the state of.
	* @param newState Pointer to the new state.
	*/
	void SetState(Guard& guard, GuardState* newState);

	/**
	* @brief Updates the current state.
	*
	* @param deltaTime The delta time of the main SDL loop.
	* @param guard Reference to the guard which to update.
	*/
	void Update(double deltaTime, Guard& guard);

	/**
	* @brief Changes the guard to the default behavior.
	*
	* @param guard Reference to the guard.
	*/
	void ChangeToDefaultState(Guard& guard);
}

#endif // GUARDSTATEMANAGER_H_