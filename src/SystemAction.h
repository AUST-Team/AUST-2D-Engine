#pragma once

#ifndef UIACTION_H_
#define UIACTION_H_

#include <string>
#include <nlohmann/json.hpp>
#include <variant>
#include <any>

/**
* @brief Structure of a system action.
*/
struct SystemAction
{
    std::string type;        /// Type of system action; e.g., "changePanel", "playAudio", "system", "changeStyle".
    nlohmann::json payload;  /// Contains parameters, key string, or nested JSON.
};

#endif // UIACTION_H_