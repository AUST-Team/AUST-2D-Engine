#pragma once

#ifndef STACKACTION_H_
#define STACKACTION_H_

#include "StackActionType.h"

class AppState;

/**
* @brief Structure for a stack action.
*/
struct StackAction
{
    StackActionType type = StackActionType::None;   /// The type of the action.
    AppState* state = nullptr;  /// The next state (only used for Push and Change).
};

#endif // !STACKACTION_H_
