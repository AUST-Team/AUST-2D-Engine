#pragma once

#ifndef INTERACTABLE_H_
#define INTERACTABLE_H_

/**
* @brief Abstract class for an interactable object
*/
class Interactable
{
public:

	/**
	* @brief Default destructor.
	*/
	virtual ~Interactable() = default;

	/**
	* @brief Interacts with an entity.
	*/
	virtual void Interact() = 0;
};

#endif // INTERACTABLE_H_