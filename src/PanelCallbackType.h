#pragma once

#ifndef PANELCALLBACKTYPE_H_
#define PANELCALLBACKTYPE_H_

#include "EnumMiscs.h"

enum class PanelCallbackType : uint8_t
{
	PanelEnter,	/// Callback for when the panel is entered.
	PanelExit,	/// Callback for when the panel is exited.
	COUNT		/// Number of elements.
};

template <>
inline constexpr bool isCountableEnum<PanelCallbackType> = true;

#endif // PANELCALLBACKTYPE_H_