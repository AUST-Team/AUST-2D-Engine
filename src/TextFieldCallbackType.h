#pragma once

#ifndef TEXTFIELDCALLBACKTYPE_H_
#define TEXTFIELDCALLBACKTYPE_H_

#include "EnumMiscs.h"

/**
* @brief Enumeration of text field callbacks types.
*/
enum class TextFieldCallbackType : uint8_t
{
	TextInputStart,	/// Text input start callback.
	TextInputStop,	/// Text input stop callback.
	COUNT			/// Number of elements.
};

template <>
inline constexpr bool isCountableEnum<TextFieldCallbackType> = true;

#endif // TEXTFIELDCALLBACKTYPE_H_