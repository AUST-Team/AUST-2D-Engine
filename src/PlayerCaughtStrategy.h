#pragma once

#ifndef PLAYERCAUGHTSTRATEGY_H_
#define PLAYERCAUGHTSTRATEGY_H_

#include <string>
#include <vector>

#include "PlayerStrategy.h"

class Guard;

/**
* @brief Strategy for when the player gets caught by the guards.
*/
class PlayerCaughtStrategy : public PlayerStrategy
{
private:
	Guard* guard = nullptr;	/// Guard that has caught the player. Non-owning.

    /**
    * @brief Default constructor.
    */
    PlayerCaughtStrategy() = default;

    /**
    * @brief Default destructor.
    */
    ~PlayerCaughtStrategy() override = default;

    // No copying or moving allowed due to singleton pattern.
    PlayerCaughtStrategy(const PlayerCaughtStrategy&) = delete;
    PlayerCaughtStrategy& operator=(const PlayerCaughtStrategy&) = delete;
    PlayerCaughtStrategy(PlayerCaughtStrategy&&) = delete;
    PlayerCaughtStrategy& operator=(PlayerCaughtStrategy&&) = delete;

public:

    /**
    * @brief Returns the created instance of the caught strategy.
    *
    * @return Reference to the caught strategy.
    */
    static PlayerCaughtStrategy& GetInstance() noexcept
    {
        static PlayerCaughtStrategy instance;
        return instance;
    }

    /**
    * @brief Updates the caught strategy.
    * 
    * @param deltaTime The delta time of the main SDL loop.
    */
    void Update(double deltaTime) override;

    /**
    * @brief Sets up the strategy.
    */
    void Setup() override;

    /**
    * @brief Handles a command.
    * 
    * @param command Command to be handled.
    */
    void HandleCommand(const Command* command) override;

    /**
    * @brief Returns the strategy type.
    *
    * @return A 'Caught' strategy type.
    */
    PlayerStrategyType GetStrategyType() const override;

    /**
    * @brief Sets the guard that has caught the player.
    * 
    * @param newGuard Pointer to the guard.
    */
    void SetGuard(Guard* newGuard);

    /**
    * @brief Returns the player that has caught the player.
    * 
    * @return Pointer to the guard that has caught the player.
    */
    Guard* GetGuard() const;
};

#endif // PLAYERCAUGHTSTRATEGY_H_
