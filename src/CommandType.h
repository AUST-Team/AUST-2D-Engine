#pragma once

#ifndef COMMANDTYPE_H_
#define COMMANDTYPE_H_

#include <cstdint>

/**
* @brief Enumeration of all command types.
*/
enum class CommandType : uint8_t
{
    Movement,   /// Movement command type.
    Action,     /// Action command type.
    Menu,       /// Menu command type.
    Debug,      /// Debug command type.
    COUNT       /// Number of elements. 
};

#endif // COMMANDTYPE_H_
