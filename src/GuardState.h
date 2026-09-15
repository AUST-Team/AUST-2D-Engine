#pragma once

#ifndef GUARDSTATE_H_
#define GUARDSTATE_H_

#include "GuardStateType.h"

class Guard;

/**
* @brief Abstract class for a guard state.
* 
* A state is shareable if it can be used simultanously by multiple guards (usually when it contains no data, or only shareable data).
*/
class GuardState
{
public:

	/**
	* @brief Default destructor.
	*/
	virtual ~GuardState() = default;

	/**
	* @brief Enters the state.
	* 
	* @param guard Reference to the guard that will enter this state.
	*/
	virtual void Enter(Guard& guard) = 0;

	/**
	* @brief Updates the state.
	*
	* @param deltaTime The deltaTime of the main SDL loop.
	* @param guard Reference to the guard that is in this state.
	*/
	virtual void Update(double deltaTime, Guard& guard) = 0;

	/**
	* @brief Exits the state.
	*
	* @param guard Reference to the guard that is about to exit the state.
	*/
	virtual void Exit(Guard& guard) = 0;

	/**
	* @brief Returns if the state can be shared by multiple guards or if the state must be unique for each guard.
	* 
	* Rule of thumb: if a state contains data, then it usually cannot be shared.
	* 
	* @return 'true' if the state can be shared, 'false' if the state is unique per guard.
	* Default 'true'.
	*/
	virtual bool IsShared() const = 0;

	/**
	* @brief Returns the state type of the state.
	* 
	* @return The state type of the current state.
	*/
	virtual GuardStateType GetStateType() const = 0;
};

#endif // GUARDSTATE_H_
