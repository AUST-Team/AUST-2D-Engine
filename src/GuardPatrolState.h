#pragma once

#ifndef GUARDPATROLSTATE_H_
#define GUARDPATROLSTATE_H_

#include "GuardState.h"

/**
* @brief Patrol state for the guards.
* 
* Uses the patrol strategy that a guard has.
* 
* Shareable.
*/
class GuardPatrolState : public GuardState
{
private:

	/**
	* @brief Default constructor.
	*/
	GuardPatrolState() = default;

	/**
	* @brief Default destructor.
	*/
	~GuardPatrolState() = default;

	// No copying or moving allowed due to singleton pattern.
	GuardPatrolState(const GuardPatrolState&) = delete;
	GuardPatrolState& operator=(const GuardPatrolState&) = delete;
	GuardPatrolState(GuardPatrolState&&) = delete;
	GuardPatrolState& operator=(GuardPatrolState&&) = delete;

public:

	/**
	* @brief Returns the created instance of the patrol state, or creates the instance and returns it.
	*
	* @return The created instance of the patrol state.
	*/
	static GuardPatrolState& GetInstance()
	{
		static GuardPatrolState instance;
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
	* @return 'true' for the patrol state.
	*/
	bool IsShared() const override;

	/**
	* @brief Returns the state type of the state.
	*
	* @return A 'Moving' state type.
	*/
	GuardStateType GetStateType() const override;
};

#endif // GUARDPATROLSTATE_H_
