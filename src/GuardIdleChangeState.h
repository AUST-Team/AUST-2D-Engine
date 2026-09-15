#pragma once

#ifndef GUARDIDLECHANGESTATE_H_
#define GUARDIDLECHANGESTATE_H_

#include "GuardState.h"

/**
* @brief Idle state of the guard where they change the state after some time.
* 
* Non-shareable.
*/
class GuardIdleChangeState : public GuardState
{
private:
	GuardState* nextState;	/// The next state to be set. Semi-owning (state-dependent).
	float idleTimer = 2000.0f;	/// Number of seconds to stay inside the idle state (miliseconds).

	// No copying allowed due to some states being uncopyable.
	GuardIdleChangeState(const GuardIdleChangeState&) = delete;
	GuardIdleChangeState& operator=(const GuardIdleChangeState&) = delete;

public:

	/**
	* @brief Constructor.
	* 
	* @param nextState The next state to switch to.
	* @param idleDurationFrames The number of seconds to stay inside the idle state (miliseconds).
	*/
	explicit GuardIdleChangeState(GuardState* nextState, float idleDurationTimer = 2000.0f);

	/**
	* @brief Move constructor.
	*/
	GuardIdleChangeState(GuardIdleChangeState&& other) noexcept;

	/**
	* @brief Destructor.
	*/
	~GuardIdleChangeState() override;

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
	* @return 'false' as the state is not shareable.
	*/
	bool IsShared() const override;

	/**
	* @brief Returns the state type of the state.
	*
	* @return An 'Idle' state type.
	*/
	GuardStateType GetStateType() const override;

	/**
	* @brief Move operator.
	*/
	GuardIdleChangeState& operator=(GuardIdleChangeState&& other) noexcept;
};


#endif // GUARDIDLECHANGESTATE_H_