#pragma once

#ifndef MISCS_H_
#define MISCS_H_

#include <functional>
#include <nlohmann/json.hpp>

#include "Direction.h"

class GuardStrategy;
class Guard;

/**
* @brief Makes a direction provider from a guard strategy.
* 
* @param strategy The strategy to use for the direction provider.
* @param guard The guard used for the strategy.
*/
std::function<Direction()> MakeDirectionProviderFrom(GuardStrategy* strategy, Guard& guard);

/**
* @brief Creates a function from a JSON.
* 
* @param callbackJSON JSON containing the parameters of the function (type, params).
* 
* @return A function that will call the event with the respective type and parameters.
*/
std::function<void()> CreateCallbackFromJSON(const nlohmann::json& callbackJSON);


#endif //MISCS_H_