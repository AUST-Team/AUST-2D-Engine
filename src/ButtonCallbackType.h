#pragma once

#ifndef BUTTONCALLBACKTYPE_H_
#define BUTTONCALLBACKTYPE_H_

#include <cstdint>

#include "EnumMiscs.h"

/**
* @brief Callback types for buttons.
*/
enum class ButtonCallbackType : uint8_t
{
	Click,		/// Callbacks for clicking.
	EnterHover,	/// Callbacks for entering the button's boundary (enter hover).
	ExitHover,	/// Callbacks for exiting the button's boundary (exit hover).
	COUNT		/// Number of elements.
};

template <>
inline constexpr bool isCountableEnum<ButtonCallbackType> = true;

#endif // BUTTONCALLBACKTYPE_H_