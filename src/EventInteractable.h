#pragma once

#ifndef EVENTINTERACTABLE_H_
#define EVENTINTERACTABLE_H_

#include <functional>

#include "Interactable.h"

class EventInteractable : public Interactable
{
private:
	std::function<void()> eventCallback = nullptr;	/// Callback to the event to trigger.

public:

	/**
	* @brief Constructor.
	* 
	* @param callback Function to call on interact.
	*/
	EventInteractable(const std::function<void()>& callback);

	/**
	* @brief Default destructor.
	*/
	~EventInteractable() override = default;

	/**
	* @brief Interacts with an entity.
	*/
	void Interact() override;
};

#endif // EVENTINTERACTABLE_H_