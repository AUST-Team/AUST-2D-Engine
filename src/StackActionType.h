#pragma once

#ifndef STACKACTIONTYPE_H_
#define STACKACTIONTYPE_H_

#include <cstdint>

/**
* @brief Enumeration of possible stack actions.
*/
enum class StackActionType : uint8_t
{
    None,       /// No operation.
    Push,       /// Push operation.
    Pop,        /// Pop operation.
    Change,     /// Change (full clear of stack) operation.
    COUNT       // Number of elements.
};

/*
* Priority (in descending order):
*   - Force changing states (ChangeState())     -> asynchronous, can change states mid-update / render / event handling.
*   - Quitting                                  -> doesn't use stack, but must be synchronous to allow safe quitting.
*   - Scheduling a stack action                 -> synchronous, does the action!
*/

#endif // STACKACTIONTYPE_H_