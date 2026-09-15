#pragma once

#ifndef OVERRIDETYPE_H_
#define OVERRIDETYPE_H_

#include <cstdint>

enum class OverrideType : uint8_t
{
	Semi,	/// Semi override. Uses shared animation state from the TileInstance.
	Full,	/// Full override. Uses own animation state.
	COUNT	/// Number of elements.
};

#endif // OVERRIDETYPE_H_