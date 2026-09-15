#pragma once

#ifndef RESOLVEDCHOICE_H_
#define RESOLVEDCHOICE_H_

#include <string>
#include <vector>

#include "SystemAction.h"

/**
* @brief Structure for a resolved choice.
*/
struct ResolvedChoice
{
    std::string text;
    std::vector<SystemAction> actions;
};

#endif // RESOLVEDCHOICE_H_