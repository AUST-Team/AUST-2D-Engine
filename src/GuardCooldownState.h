#pragma once

#ifndef GUARDCOOLDOWNSTATE_H_
#define GUARDCOOLDOWNSTATE_H_

#include "GuardState.h"

/**
* @brief Cooldown state for the guard, used after the guard has caught the player.
* 
* Force sets the vision and aggression to normal, and makes the guard 'blind' and idle. Changes to default state after.
* 
* Non-shareable.
*/
class GuardCooldownState : public GuardState
{
private:
	float cooldownTimer = 5000.0f;	/// Seconds to stay in the cooldown state (in miliseconds).

public:

	/**
	* @brief Constructor.
	* 
	* @param cooldownFrames The number of seconds to stay in the cooldown state (stated in miliseconds). -1 for infinite cooldown.
	*/
	explicit GuardCooldownState(float cooldownDurationTimer = 5000.0f);

	/**
	* @brief Default destructor.
	*/
	~GuardCooldownState() override = default;

	/**
	* @brief Enters the state.
	*
	* @param guard Reference to the guard that will enter this state.
	*/
	void Enter(Guard& guard) override;

	/**
	* @brief Updates the state.
	*
	* @param deltaTime The deltaTime of the main SDL loop.
	* @param guard Reference to the guard that is in this state.
	*/
	void Update(double deltaTime, Guard& guard) override;

	/**
	* @brief Exits the state.
	*
	* @param guard Reference to the guard that is about to exit the state.
	*/
	void Exit(Guard& guard) override;

	/**
	* @brief Returns if the state can be shared by multiple guards or if the state must be unique for each guard.
	*
	* @returns 'false' as the class is not shareable.
	*/
	bool IsShared() const override;

	/**
	* @brief Returns the state type of the state.
	*
	* @return An 'Alert' state type.
	*/
	GuardStateType GetStateType() const override;
};

#endif // GUARDCOOLDOWNSTATE_H_
