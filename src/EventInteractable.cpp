#include "EventInteractable.h"

EventInteractable::EventInteractable(const std::function<void()>& callback) :
	eventCallback(callback) {}

void EventInteractable::Interact()
{
	if (eventCallback)
	{
		eventCallback();
	}
}
