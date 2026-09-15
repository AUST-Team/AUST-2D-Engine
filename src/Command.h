#pragma once

#ifndef COMMAND_H
#define COMMAND_H

#include "CommandType.h"

/**
* @brief Interface for the Command pattern.
*/
class Command 
{
public:

    /**
    * @brief Default destructor.
    */
    virtual ~Command() = default;

    /**
    * @brief Executes a command.
    */
    virtual void Execute() = 0;

    /**
    * @brief Returns the CommandType for the object
    * 
    * @return The CommandType of the object.
    */
    virtual CommandType GetType() const = 0;
};

#endif