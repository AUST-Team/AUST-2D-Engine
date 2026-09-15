#pragma once

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "Command.h"
#include "Direction.h"

/**
* Encapsulates a movement command.
*/
class MoveCommand : public Command
{
private:
	Direction direction;

public:

	/**
	* @brief Constructor.
	* 
	* @param direction Direction of the movement.
	*/
	explicit MoveCommand(Direction direction);

	/**
	* @brief Default destructor.
	*/
	~MoveCommand() override = default;

	/**
	* @brief Executes the move command.
	*/
	void Execute() override;

	/**
	* @brief Returns the type of the command.
	* 
	* @return Returns CommandType::Movement.
	*/
	CommandType GetType() const override;

	/**
	* @brief Returns the given direction of the movement.
	* 
	* @return The direction of the move command.
	*/
	Direction GetDirection() const;
};

#endif // COMMANDS_H_