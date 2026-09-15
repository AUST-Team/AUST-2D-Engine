#pragma once

#ifndef TRIGGERFACTORY_H_
#define TRIGGERFACTORY_H_

#include <nlohmann/json.hpp>

#include "TriggerFlags.h"

class Trigger;
class ConditionRegistry;

/**
* @brief Namespace for a trigger factory.
*/
namespace TriggerFactory
{
	/**
	* @brief Create a trigger from parameters.
	* 
	* @param triggerJSON JSON containing the description (parameters) of the trigger.
	* @param conditionRegistry The condition registry of the map.
	* 
	* @return Pointer to the created trigger.
	*/
	Trigger* CreateTriggerFromJSON(const nlohmann::json& triggerJSON, ConditionRegistry& conditionRegistry);

	/**
	* @brief Creates trigger flags from parameters.
	* 
	* @param flagsJSON JSON containing the flag parameters of the trigger.
	* @param trigger Pointer to the trigger to set the flags. (YOU WON'T LOSE OWNERSHIP!)
	*/
	void SetTriggerFlagsFromJSON(const nlohmann::json& flagsJSON, Trigger* trigger);
}

#endif // TRIGGERFACTORY_H_