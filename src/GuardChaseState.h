#pragma once

#ifndef GUARDCHASESTATE_H_
#define GUARDCHASESTATE_H_

#include "GuardState.h"
#include "GuardPathChaseStrategy.h"

/**
* @brief Chase state for the guard.
* 
* Shareable.
*/
class GuardChaseState : public GuardState
{
private:
	GuardPathChaseStrategy chaseStrategy;

	/**
	* @brief Default constructor.
	*/
	GuardChaseState() = default;

	/**
	* @brief Default destructor.
	*/
	~GuardChaseState() override = default;

	// No copying or moving allowed due to singleton pattern.
	GuardChaseState(const GuardChaseState&) = delete;
	GuardChaseState& operator=(const GuardChaseState&) = delete;
	GuardChaseState(GuardChaseState&&) = delete;
	GuardChaseState& operator=(GuardChaseState&&) = delete;

	/**
	* @brief Checks if the guard has caught the player.
	*
	* @param guard Reference to the guard.
	*/
	bool HasCaughtPlayer(Guard& guard);

public:

	/**
	* @brief Returns the instance of the state or creates it and returns it.
	*
	* @return Referece to the created state.
	*/
	static GuardChaseState& GetInstance()
	{
		static GuardChaseState instance;
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
	* @return 'true' for the chase state.
	*/
	bool IsShared() const override;

	/**
	* @brief Returns the state type of the state.
	*
	* @return An 'Alert' state type.
	*/
	GuardStateType GetStateType() const override;
};

#endif // GUARDCHASESTATE_H_
