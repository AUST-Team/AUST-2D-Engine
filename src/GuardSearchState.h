#pragma once

#ifndef GUARDSEARCHSTATE_H_
#define GUARDSEARCHSTATE_H_

#include <vector>

#include "GuardState.h"
#include "Direction.h"

class GuardStrategy;
struct SDL_FPoint;

/**
* @brief Search state for the guard.
* 
* Non-shareable.
*/
class GuardSearchState : public GuardState
{
private:
	std::vector<GuardStrategy *> strategies;	/// The search strategies. OWNED!
	size_t currentIndex = 0;	/// The current strategy the guard is at.

	// No copying allowed due to... gonna be honest I don't want to deal with polymorhpic coyping.
	GuardSearchState(const GuardSearchState&) = delete;
	GuardSearchState& operator=(const GuardSearchState&) = delete;

	/**
	* @brief Checks if the current strategy is done.
	* 
	* @param strategy The strategy to check.
	* 
	* @return 'true' if the strategy is done, or if the strategy is not a search strategy, 'false' if the search strategy is not done.
	*/
	bool IsStrategyDone(GuardStrategy* strategy);

public:

	/**
	* @brief Constructor.
	* 
	* @param lastSeenX The X coordinate of the last position (tile index) the player was spotted at.
	* @param lastSeenY The Y coordinate of the last position (tile index) the player was spotted at.
	* @param lastSeenDir The last direction the player was spotted walking in.
	* @param aggressionLevel The aggression level of the guard.
	*/
	GuardSearchState(int lastSeenX, int lastSeenY, Direction lastSeenDir, float aggressionLevel);

	/**
	* @brief Constructor.
	*
	* @param lastSeenPoint The coordinates of the last position (tile index) the player was spotted at.
	* @param lastSeenDir The last direction the player was spotted walking in.
	* @param aggressionLevel The aggression level of the guard.
	*/
	GuardSearchState(const SDL_Point& lastSeenPoint, Direction lastSeenDir, float aggressionLevel);

	/**
	* @brief Move constructor.
	*/
	GuardSearchState(GuardSearchState&& other) noexcept;

	/**
	* @brief Destructor.
	*/
	~GuardSearchState() override;

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
	* @return 'false' as the class is not shareable.
	*/
	bool IsShared() const override;

	/**
	* @brief Returns the state type of the state.
	*
	* @return An 'Alert' state type.
	*/
	GuardStateType GetStateType() const override;

	/**
	* @brief Move operator.
	*/
	GuardSearchState& operator=(GuardSearchState&& other) noexcept;
};

#endif // GUARDSEARCHSTATE_H_