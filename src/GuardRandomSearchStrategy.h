#pragma once

#ifndef GUARDRANDOMSEARCHSTRATEGY_H_
#define GUARDRANDOMSEARCHSTRATEGY_H_

#include "GuardStrategy.h"
#include "GuardSearchStrategyProgressTracker.h"

/**
* @brief Random search strategy for a guard. Chooses a new direction every given cooldown.
* 
* No pathfinding.
* 
* Non-shareable.
*/
class GuardRandomSearchStrategy : public GuardStrategy, public GuardSearchStrategyProgressTracker
{
private:
	float searchTimer = 0.0f;		/// Timer for the search.
	float searchDuration = 180.0f;	/// The maximum search duration time. Miliseconds.
	Direction currentDirection;		/// Current walking direction.
	float changeCooldown = 30.0f;	/// Cooldown of direction changes. Miliseconds.

	/**
	* @brief Checks if the 'first' direction is opposite of the 'second' direction.
	* 
	* @param first The first direction.
	* @param second The second direction.
	* 
	* @return 'true' if the first direction is the opposite of the second direction, 'false' if otherwise.
	*/
	bool IsDirectionOpposite(Direction first, Direction second) const;

	/**
	* @brief Returns a random direction.
	* 
	* @return A direction.
	*/
	Direction GetRandomDirection() const;

public:

	/**
	* @brief Constructor.
	* 
	* @param searchTime The search time. Miliseconds.
	* @param dirChangeCooldown The number of miliseconds after which a direction change happens.
	*/
	explicit GuardRandomSearchStrategy(float searchTime = 180.0f, float dirChangeCooldown = 30.0f);

	/**
	* @brief Default destructor.
	*/
	~GuardRandomSearchStrategy() override = default;

	/**
	* @brief Returns the next direction for the guard to take.
	*
	* @param guard Reference to the guard.
	*
	* @return A direction for the guard to take, or Direction::ENUM_SENTINEL_VALUE if the guard should not move, or is stuck.
	*/
	Direction GetDirection(Guard& guard) override;

	/**
	* @brief Updates the strategy.
	*
	* @param delteTime The delta time of the main game loop.
	*/
	void Update(double deltaTime) override;

	/**
	* @brief Returns the status of the search strategy.
	*
	* @return 'true' if the strategy has finished, 'false' if otherwise.
	*/
	bool IsSearchStrategyDone() const override;
};

#endif // GUARDRANDOMSEARCHSTRATEGY_H_