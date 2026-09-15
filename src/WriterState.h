#pragma once

#ifndef WRITERSTATE_H_
#define WRITERSTATE_H_

#include <functional>
#include <SDL3/SDL_events.h>

/**
* @brief Structure for a writer state.
*/
struct WriterState
{
	std::function<void(uint32_t, uint32_t)> onUserInteract = nullptr;	/// Function to call on user interact.
	std::function<void(double)> onUpdate = nullptr;			/// Function to call on updating.
};

#endif // WRITERSTATE_H_