#pragma once

#ifndef INTERACTCOMMAND_H_
#define INTERACTCOMMAND_H_

#include "Command.h"

/**
* @brief Command to interact with any interactables around the player.
*/
class InteractCommand : public Command
{
public:

	/**
	* @brief Default destructor.
	*/
	~InteractCommand() override = default;

	/**
	* @brief Executes the interact command.
	*/
	void Execute() override;

	/**
	* @brief Returns the command type.
	* 
	* @return An 'Action' command type.
	*/
	CommandType GetType() const override;
};

#endif // INTERACTCOMMAND_H_