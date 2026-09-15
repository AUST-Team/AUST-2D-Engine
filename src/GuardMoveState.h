#pragma once

#ifndef GUARDMOVESTATE_H_
#define GUARDMOVESTATE_H_

#include "GuardState.h"

/**
* @brief State for when the guard moves to a point.
* 
* Shareable.
*/
class GuardMoveState : public GuardState
{
private:

	/**
	* @brief Default constructor.
	*/
	GuardMoveState() = default;

	/**
	* @brief Default destructor.
	*/
	~GuardMoveState() override = default;

	// No copying or moving allowed due to singleton pattern.
	GuardMoveState(const GuardMoveState&) = delete;
	GuardMoveState& operator=(const GuardMoveState&) = delete;
	GuardMoveState(GuardMoveState&&) = delete;
	GuardMoveState& operator=(GuardMoveState&&) = delete;

public:

	/**
	* @brief Returns the created instance of the idle stay state, or creates the instance and returns it.
	*
	* @return The created instance of the idle stay state.
	*/
	static GuardMoveState& GetInstance() noexcept
	{
		static GuardMoveState instance;
		return instance;
	}

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
	* @return 'true' for the move state.
	*/
	bool IsShared() const override;

	/**
	* @brief Returns the state type of the state.
	*
	* @return A 'Moving' state type.
	*/
	GuardStateType GetStateType() const override;
};

#endif // GUARDMOVESTATE_H_
