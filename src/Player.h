#pragma once

#ifndef PLAYER_H_
#define PLAYER_H_

#include <vector>

#include "Entity.h"
#include "PlayerTileMover.h"
#include "TileInstance.h"
#include "TileReactionType.h"

class Observer;
class PlayerStrategy;

/**
* @brief Encapsulates the player.
*/
class Player : public Entity
{
private:
    static Player* playerInstance;  /// Keeps the pointer to the instance
    static bool created;            /// Flag if the instance was created or not

    std::vector<Observer*> observers = {}; /// Vector of observers. It doesn't own the pointers, just references them.

    PlayerTileMover tileMover; /// Tile mover

    PlayerStrategy* currentStrategy = nullptr;	/// Current strategy of the player.
    TileAnimationType animationOverride;    /// Animation override.
    int timesCaught = 0;    /// The number of times the player has been caught.

    // No copying or moving allowed due to singleton pattern.
    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;
    Player(Player&&) = delete;
    Player& operator=(Player&&) = delete;

public:

    /**
    * @brief Constructor. Puts the player at a certain position.
    *
    * @param startX X coordinate (tile index) of the player start.
    * @param startY Y coordinate (tile index) of the player start.
    */
    explicit Player(int startX = 0, int startY = 0);

    /**
    * @brief Destructor.
    *
    * Deletion is done through the DeleteInstance() method.
    */
    ~Player() override;

    /**
    * @brief Creates an instance of a Player object.
    * 
    * @param startX X coordinate (tile index) of the player start.
    * @param startY Y coordinate (tile index) of the player start.
    * 
    * @return The pointer to the instance.
    */
    static Player* CreateInstance(int startX = 0, int startY = 0);

    /**
    * @brief Creates an instance of a Player object.
    *
    * @param startPoint The starting point (tile indexes) of the player.
    *
    * @return The pointer to the instance.
    */
    static Player* CreateInstance(const SDL_Point& startPoint = { 0, 0 });

    /**
    * @brief Returns the instance of the Player object.
    * 
    * @return Pointer to the Player object
    */
    static Player* GetInstance();

    /**
    * @brief Deletes the instance of the Player object
    */
    static void DeleteInstance();

    /**
    * @brief Updates the player every game tick.
    * 
    * @param deltaTime The delta time of the game loop.
    */
    void Update(double deltaTime) override;

    /**
    * @brief Adds an observer to the player.
    * 
    * @param o Pointer to the observer to be added.
    */
    void AddObserver(Observer* o);

    /**
    * @brief Removes an observer from the player.
    * 
    * @param o Pointer to the observer to be removed.
    */
    void RemoveObserver(Observer* o);

    /**
    * @brief Clears all observers from the player.
    */
    void ClearObservers();

    /**
    * @brief Sets the position of the player.
    * 
    * @param x The X coordinate of the new position.
    * @param y The Y coordinate of the new position.
    */
    void SetPosition(float x, float y) override;

    /**
    * @brief Sets the position of the player.
    *
    * Snaps to the nearest tile.
    *
    * @param newPosition The new position of the player.
    */
    void SetPosition(const SDL_FPoint& newPosition) override;

    /**
    * @brief Sets the position of the player in relation to tiles on the map.
    *
    * Snaps to the nearest tile.
    *
    * @param x The X coordinate of the tile.
    * @param y The Y coordinate of the tile.
    */
    void SetPositionTile(int x, int y);

    /**
    * @brief Sets the position of the player in relation to tiles on the map.
    *
    * Snaps to the nearest tile.
    *
    * @param newPosition The new position (tile indexes) of the guard.
    */
    void SetPositionTile(const SDL_Point& newPosition);

    /**
    * @brief Resets the movement state of the player.
    * The player will stop moving and any target tile will be set to the last tile they were on.
    */
    void ResetMovementState();

    /**
    * @brief Returns the X coordinate of the player.
    * 
    * @return The X coordinate of the player.
    */
    float GetX() const override;

    /**
    * @brief Returns the Y coordinate of the player.
    *
    * @return The Y coordinate of the player.
    */
    float GetY() const override;

    /**
    * @brief Returns the X and Y coordinate of the player.
    * 
    * @return The SDL_FPoint containing the position of the player.
    */
    SDL_FPoint GetPosition() const override;

    /**
    * @brief Returns the speed of the player.
    * 
    * @return The speed of the player.
    */
    float GetSpeed() const override;

    /**
    * @brief Returns the column of the tile the player is on.
    * 
    * @return The column of the tile the player is on.
    */
    int GetTileX() const;

    /**
    * @brief Returns the row of the tile the player is on.
    *
    * @return The row of the tile the player is on.
    */
    int GetTileY() const;

    /**
    * @brief Returns the X and Y coordinate of the tile the player is on.
    *
    * @return The SDL_FPoint containing the tile position of the player.
    */
    SDL_Point GetTilePosition() const override;

    /**
    * @brief Returns if the player is moving.
    * 
    * @return true if the player is moving, false if otherwise.
    */
    bool IsMoving() const;

    /**
    * @brief Sets the facing direction of the player (mover).
    *
    * Usually not needed to be called manually.
    *
    * @param newDirection New facing direction of the player.
    */
    void SetLastDirection(Direction newDirection);

    /**
    * @brief Returns the last direction of the player.
    * 
	* @return The last direction of the player, or Direction::ENUM_SENTINEL_VALUE if the player has not moved yet.
    */
    Direction GetLastDirection() const;

    /**
    * @brief Returns the number of times the player has been caught.
    *
    * @return The number of times the player has been caught.
    */
    int GetTimesCaught() const;

    /**
    * @brief Increases the number of times the player has been caught by one.
    */
    void IncrementTimesCaught();

    /**
    * @brief Set the times caught.
    * 
    * Used for saving / loading, use increment method for anything else.
    * 
    * @param newValue New value for the number of times the player has been caught.
    */
    void SetTimesCaught(int newValue);

    /**
    * @brief Tries to move the player in the direction specified.
    * 
    * @param dir Direction to move in.
    */
    void TryMove(Direction dir);

    /**
    * @brief Sets the active movement.
    *
    * @param value 'true' if the player should be able to move, 'false' if otherwise.
    */
    void SetActiveMovement(bool value);

    /**
    * @brief Sets the player strategy.
    * 
    * This sets without checks, use the strategy manager for checks.
    * 
    * @param newStrategy The new strategy for the player.
    */
    void SetCurrentStrategy(PlayerStrategy* newStrategy);

    /**
    * @brief Returns the current player strategy.
    * 
    * @return The current player strategy.
    */
    PlayerStrategy* GetCurrentStrategy();

    /**
    * @brief Sets the animation override of the player.
    * 
    * @param override Animation override of the player.
    */
    void SetAnimationOverride(TileAnimationType override);

    /**
    * @brief Returns the animation override of the player.
    * 
    * @param The type of animation override of the player.
    */
    TileAnimationType GetAnimationOverride() const;
};

#endif // PLAYER_H_