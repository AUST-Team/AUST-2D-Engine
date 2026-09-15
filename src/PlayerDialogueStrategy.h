#pragma once

#ifndef PLAYERDIALOGUESTRATEGY_H_
#define PLAYERDIALOGUESTRATEGY_H_

#include "PlayerStrategy.h"

/**
* @brief Dialogue for the player strategy.
*/
class PlayerDialogueStrategy : public PlayerStrategy
{
private:

	/**
	* @brief Default constructor.
	*/
	PlayerDialogueStrategy() = default;

	/**
	* @brief Default destructor.
	*/
	~PlayerDialogueStrategy() override = default;

	// No copying or moving allowed due to singleton pattern.
	PlayerDialogueStrategy(const PlayerDialogueStrategy&) = delete;
	PlayerDialogueStrategy& operator=(const PlayerDialogueStrategy&) = delete;
	PlayerDialogueStrategy(PlayerDialogueStrategy&&) = delete;
	PlayerDialogueStrategy& operator=(PlayerDialogueStrategy&&) = delete;

public:

	/**
	* @brief Returns the created instance of the dialogue strategy.
	* 
	* @return Reference to the dialogue strategy.
	*/
	static PlayerDialogueStrategy& GetInstance() noexcept
	{
		static PlayerDialogueStrategy instance;
		return instance;
	}

	/**
	* @brief Updates the dialogue strategy.
	*
	* @param deltaTime The delta time of the main SDL loop.
	*/
	void Update(double deltaTime) override;

	/**
	* @brief Sets up the dialogue strategy. Should only be executed once per strategy change.
	*/
	void Setup() override;

	/**
	* @brief Handles a command.
	*
	* @param command Pointer to the command.
	*/
	void HandleCommand(const Command* command) override;

	/**
	* @brief Returns the strategy type.
	*
	* @return A 'Dialogue' strategy type.
	*/
	PlayerStrategyType GetStrategyType() const override;
};

#endif // PLAYERDIALOGUESTRATEGY_H_
