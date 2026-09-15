#pragma once

#ifndef MAPTRANSITIONINTERACTABLE_H_
#define MAPTRANSITIONINTERACTABLE_H_

#include <string>

#include "Interactable.h"

class MapTransitionInteractable : public Interactable
{
private:
	std::string mapName;	/// Map name to transition to.

public:

	/**
	* @brief Default destructor.
	*/
	~MapTransitionInteractable() override = default;

	/**
	* @brief Interacts with an entity.
	*/
	void Interact() override;
};

#endif // MAPTRANSITIONINTERACTABLE_H_