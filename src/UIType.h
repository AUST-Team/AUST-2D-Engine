#pragma once

#ifndef UITYPE_H_
#define UITYPE_H_

#include <cstdint>

/**
* @brief Enumeration of UI types.
*/
enum class UIType : uint8_t
{
	MainMenu,	/// Main menu UI.
	PauseMenu,	/// Pause menu UI.
	COUNT		/// Number of elements.
};

#endif // UITYPE_H_