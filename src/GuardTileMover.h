#pragma once

#ifndef GUARDTILEMOVER_H_
#define GUARDTILEMOVER_H_

#include <functional>
#include <optional>

#include "Direction.h"
#include "TileMover.h"

struct SDL_FPoint;
class Guard;

/**
* @brief A tile mover for the guard enemy.
*/
class GuardTileMover : public TileMover
{
private:
	Guard& guard;	/// Back reference to the guard.

	std::function<Direction()> directionProvider = nullptr;	/// Provides the direction for the tile mover.

	Direction currentDirection = GetSentinel<Direction>();	/// Current direction of the guard.

	// No coyping / moving operator allowed due to references.
	GuardTileMover& operator=(GuardTileMover&&) = delete;
	GuardTileMover& operator=(const GuardTileMover&) = delete;

	/**
	* @brief Moves the guard towards the target.
	* 
	* @param deltaTime The deltaTime of the main game loop.
	*/
	void MoveTowardsTarget(double deltaTime) override;

public:

	/**
	* @brief Constructor.
	*
	* @param startX The starting X coordinate of the guard.
	* @param startY The starting Y coordinate of the guard.
	* @param guard Reference to the guard.
	*/
	GuardTileMover(float startX, float startY, Guard& guard);

	/**
	* @brief Constructor.
	*
	* @param position The starting position of the guard.
	* @param guard Reference to the guard.
	*/
	GuardTileMover(const SDL_FPoint& position, Guard& guard);

	/**
	* @brief Default copy constructor.
	*/
	GuardTileMover(const GuardTileMover& other) = default;

	/**
	* @brief Default move constructor.
	*/
	GuardTileMover(GuardTileMover&& other) noexcept = default;

	/**
	* @brief Default destructor.
	*/
	~GuardTileMover() override = default;

	/**
	* @brief Updates the mover.
	*
	* @param deltaTime The delta time of the main SDL loop.
	*/
	void Update(double deltaTime) override;

	/**
	* @brief Tries to move in the given direction.
	*
	* Queues direction if already moving.
	*
	* @param dir Direction of where to go.
	*/
	void TryMove(Direction dir) override;

	/**
	* @brief Sets the speed of the guard.
	*
	* @param newSpeed The new speed value for the guard.
	*/
	void SetSpeed(float newSpeed);

	/**
	* @brief Sets the direction provider for the mover.
	*
	* @param provider The new direction provider.
	*/
	void SetDirectionProvider(const std::function<Direction()>& provider);

	/**
	* @brief Returns the current direction of the guard.
	* 
	* @return The current direction of the guard, or Direction::ENUM_SENTINEL_VALUE if the guard is not moving or has no direction.
	*/
	Direction GetCurrentDirection() const;

	/**
	* @brief Sets the current facing direction of the guard.
	* 
	* Usually not necessary to be called explicitely.
	* 
	* @param newDirection The new direction to set.
	*/
	void SetCurrentDirection(Direction newDirection);
};
#endif // GUARDTILEMOVER_H_
