#pragma once

#ifndef PLAYERSTRATEGYTYPE_H_
#define PLAYERSTRATEGYTYPE_H_

#include <cstdint>

/**
* @brief Emumeration of the player strategy types.
*/
enum class PlayerStrategyType : uint8_t
{
	Normal,		/// Normal (moving, able to interact, etc) strategy.
	Dialogue,	/// Dialogue (talking) strategy.
	Caught,		/// Caught (forced dialogue) strategy.
	COUNT		/// Number of elements.
};

#endif // PLAYERSTRATEGYTYPE_H_
