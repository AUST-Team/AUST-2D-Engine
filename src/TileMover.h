#pragma once

#ifndef TILEMOVER_H_
#define TILEMOVER_H_

#include <SDL3/SDL_rect.h>

#include "Direction.h"

/**
* @brief Abstract class for a tile mover.
*/
class TileMover
{
protected:
	SDL_FPoint position = { 0.0f, 0.0f };		/// The current position of the entity.
	SDL_FPoint targetPosition = { 0.0f, 0.0f };	/// The position of the target;
	float speed;	/// The speed of the entity.

	bool isMovingFlag = false;	/// Flag if the entity is moving;

	/**
	* @brief Moves the entity towards the actual target.
	* 
	* @param deltaTime The deltaTime of the main game loop.
	*/
	virtual void MoveTowardsTarget(double deltaTime) = 0;

public:

	/**
	* @brief Constructor.
	*
	* @param startX The starting X coordinate of the entity.
	* @param startY The starting Y coordinate of the entity.
	*/
	explicit TileMover(float startX = 0.0f, float startY = 0.0f);

	/**
	* @brief Constructor.
	*
	* @param position The starting position of the entity.
	*/
	explicit TileMover(const SDL_FPoint& position);

	/**
	* @brief Default destructor.
	*/
	virtual ~TileMover() = default;

	/**
	* @brief Updates the mover.
	*
	* @param deltaTime The delta time of the main SDL loop.
	*/
	virtual void Update(double deltaTime) = 0;

	/**
	* @brief Tries to move in a certain direction.
	*
	* @param dir The direction to move towards.
	*/
	virtual void TryMove(Direction dir);

	/**
	* @brief Snaps the entity to the nearest tile.
	*/
	virtual void SnapToTile();

	/**
	* @brief Sets the position of the entity.
	*
	* Snaps to the nearest tile.
	*
	* @param x The new X coordinate of the entity.
	* @param y The new Y coordinate of the entity.
	*/
	void SetPosition(float x, float y);

	/**
	* @brief Sets the position of the entity.
	*
	* Snaps to the nearest tile.
	*
	* @param newPosition The new position of the entity.
	*/
	void SetPosition(const SDL_FPoint& newPosition);

	/**
	* @brief Returns the X coordinate of the entity.
	*
	* @return The X coordinate of the entity.
	*/
	float GetX() const;

	/**
	* @brief Returns the Y coordinate of the entity.
	*
	* @return The Y coordinate of the entity.
	*/
	float GetY() const;

	/**
	* @brief Returns the speed of the entity.
	*
	* @return The speed of the entity.
	*/
	float GetSpeed() const;

	/**
	* @brief Returns if the entity is currently moving or not.
	*
	* @return The isMovingFlag state.
	*/
	bool IsMoving() const;
};

#endif // TILEMOVER_H_
