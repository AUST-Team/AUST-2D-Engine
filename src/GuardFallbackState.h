#pragma once

#ifndef GUARDFALLBACKSTATE_H_
#define GUARDFALLBACKSTATE_H_

#include "GuardState.h"

/**
* @brief Class for a fallback state.
* 
* Makes the guard simply stay put whereever they may be, chases the player if they see him.
* 
* Shareable.
*/
class GuardFallbackState : public GuardState
{
private:

	/**
	* @brief Default constructor.
	*/
	GuardFallbackState() = default;

	/**
	* @brief Default destructor.
	*/
	~GuardFallbackState() override = default;

	// No copying or moving allowed due to singleton pattern.
	GuardFallbackState(const GuardFallbackState&) = delete;
	GuardFallbackState& operator=(const GuardFallbackState&) = delete;
	GuardFallbackState(GuardFallbackState&&) = delete;
	GuardFallbackState& operator=(GuardFallbackState&&) = delete;

public:

	/**
	* @brief Returns the instance of the state or creates it and returns it.
	* 
	* @return Referece to the created state.
	*/
	static GuardFallbackState& GetInstance()
	{
		static GuardFallbackState instance;
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
	* @return 'true' for the fallback state.
	*/
	bool IsShared() const override;

	/**
	* @brief Returns the state type of the state.
	*
	* @return An 'Idle' state type.
	*/
	GuardStateType GetStateType() const override;
};

#endif // GUARDFALLBACKSTATE_H_
