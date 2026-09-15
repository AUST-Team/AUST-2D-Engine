#pragma once

#ifndef PLAYERTILEMOVER_H_
#define PLAYERTILEMOVER_H_

#include "TileMover.h"

class Player;
class KeyManager;

/**
* @brief Encapsulates the player movement logic.
*/
class PlayerTileMover : public TileMover
{
private:
    Player& player;         /// Reference to the player.
    KeyManager& keyManager; /// Reference to the key manager.

    Direction queuedDirection = GetSentinel<Direction>();   /// Queued direction (used for smooth movement).
    Direction lastDirection = GetSentinel<Direction>();     /// Last direction walked (used for enemies).

    bool isActiveFlag = true;   /// Flag if the mover is active.
    bool playerDirectionChangedFlag = false;    /// Flag if the player has changed direction.
    bool playerPositionChangedFlag = false;     /// Flag if the player has changed position.

    // No assignment operators allowed due to references.
    PlayerTileMover& operator=(const PlayerTileMover&) = delete;
    PlayerTileMover& operator=(PlayerTileMover&&) = delete;

    /**
    * @brief Moves the player towards the actual target.
    * 
    * @param deltaTime The delta time of the main game loop.
    */
    void MoveTowardsTarget(double deltaTime) override;

    /**
    * @brief Polls the key manager and tries to move immediately.
    * 
    * Used to keep movement smooth between tiles.
    */
    void PollKeyManagerAndMove();

    /**
    * @brief Helper function to determine whether a direction (key) is still held or not.
    * 
    * @return true if the direction is still held, false otherwise.
    */
    bool IsDirectionHeld(Direction dir);

    /**
    * @brief Converts a key code to the respective direction.
    * 
	* @return The Direction of the key code if it a movement key, Direction::ENUM_SENTINEL_VALUE if otherwise.
    */
    Direction KeyToDirection(int keyCode);

public:

    /**
    * @brief Constructor.
    * 
    * The reference to the player is needed as the Player circularly depends on PlayerTileMover, and thus, Player::GetInstance() will be null if called right now.
    * 
    * @param startX The starting X coordinate of the player.
    * @param startY The starting Y coordinate of the player.
    * @param player A reference to the player.
    */
    PlayerTileMover(float startX, float startY, Player& player);

    /**
    * @brief Constructor.
    * 
    * @param position Starting position to the player.
    * @param player Reference to the player.
    */
    PlayerTileMover(const SDL_FPoint& position, Player& player);

    /**
    * @brief Default copy constructor.
    */
    PlayerTileMover(const PlayerTileMover& other) = default;

    /**
    * @brief Default move constructor.
    */
    PlayerTileMover(PlayerTileMover&& other) noexcept = default;
    
    /**
    * @brief Default destructor.
    */
    ~PlayerTileMover() override = default;

    /**
    * @brief Updates the mover.
    * 
    * Doesn't do anything is the isActive flag is false. Will move to target otherwise.
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
    * @brief Resets the movement state of the mover.
    * 
    * Useful for saving / loading.
    */
    void ResetMovementState();

    /**
    * @brief Sets the facing direction of the player (mover).
    * 
    * Usually not needed to be called manually.
    * 
    * @param newDirection New facign direction of the player.
    */
    void SetLastDirection(Direction newDirection);

    /**
    * @brief Returns the last direction the player went to.
    * 
	* @return The last direction of the player, or Direction::ENUM_SENTINEL_VALUE if the player hasn't moved yet.
    */
    Direction GetLastDirection() const;

    /**
    * @brief Sets the tile mover active flag.
    * 
    * @param newValue 'true' if the tile mover should be active, 'false' if otherwise.
    */
    void SetActive(bool newValue);
};

#endif // PLAYERTILEMOVER_H_
