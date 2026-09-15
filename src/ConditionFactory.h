#pragma once

#ifndef CONDITIONFACTORY_H_
#define CONDITIONFACTORY_H_

#include <nlohmann/json.hpp>

#include "ConditionKey.h"

class Condition;
class ConditionRegistry;

/**
* @brief Namespace for functions that create conditions.
*/
namespace ConditionFactory
{
	/**
	* @brief Creates a condition from a JSON file.
	* 
	* @param conditionJSON JSON file containing the condition description.
	* @param conditionRegistry Reference to the condition registry.
	* 
	* @return Pointer to the condition created.
	*/
	Condition* CreateConditionFromJSON(const nlohmann::json& conditionJSON, ConditionRegistry& conditionRegistry);

	/**
	* @brief Creates a condition from a condition key.
	*
	* @param key Key of the condition.
	*
	* @return Pointer to the condition created.
	*/
	Condition* CreateConditionFromKey(ConditionKey key);
}

#endif // CONDITIONFACTORY_H_