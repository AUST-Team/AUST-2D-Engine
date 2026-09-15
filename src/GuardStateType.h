#pragma once

#ifndef GUARDSTATETYPE_H_
#define GUARDSTATETYPE_H_

#include <cstdint>

/**
* @brief Enumeration of state types for the guard states.
*/
enum class GuardStateType : uint8_t
{
	Alert,	/// Alert state; chasing or searching for the player.
	Idle,	/// Idle state; changing or idle stay.
	Moving,	/// Moving state; patrol or move.
	COUNT	/// Number of elements.
};

#endif // GUARDSTATETYPE_H_