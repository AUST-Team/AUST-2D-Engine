#pragma once

#ifndef TOGGLEPAUSECOMMAND_H_
#define TOGGLEPAUSECOMMAND_H_

#include "Command.h"

/**
* @brief Toggles the pause menu on and off.
*/
class TogglePauseCommand : public Command
{
public:

	/**
	* @brief Default destructor.
	*/
	~TogglePauseCommand() override = default;

	/**
	* @brief Executes the toggle pause command.
	*/
	void Execute() override;

	/**
	* @brief Returns the command type.
	* 
	* @return A CommandType::Menu value.
	*/
	CommandType GetType() const override;
};

#endif // TOGGLEPAUSECOMMAND_H_