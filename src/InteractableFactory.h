#pragma once

#ifndef INTERACTABLEFACTORY_H_
#define INTERACTABLEFACTORY_H_

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

class Interactable;

/**
* @brief Namespace for an interactable factory.
*/
namespace InteractableFactory
{
	/**
	* @brief Creates an interactable from the name.
	* 
	* @param interactableJSON JSON containing the type and parameters of the interactable.
	* 
	* @return Pointer to the interactible created, or nullptr if the type doesn't exist.
	*/
	Interactable* CreateInteractableFromJSON(const nlohmann::json& interactableJSON);
}

#endif // INTERACTABLEFACTORY_H_