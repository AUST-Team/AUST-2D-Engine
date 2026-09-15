#pragma once

#ifndef SDL_FRECTOPERATORS_H_
#define SDL_FRECTOPERATORS_H_

#include <SDL3/SDL_rect.h>
#include <cmath>

#include "FloatUtils.h"

/**
* @brief Equality operator for SDL_FRects.
* 
* @return 'true' if the first element is equal to the second, 'false' if otherwise.
*/
inline bool operator==(const SDL_FRect& a, const SDL_FRect& b)
{
	return FloatUtils::IsEqualF(a.x, b.x)
		&& FloatUtils::IsEqualF(a.y, b.y)
		&& FloatUtils::IsEqualF(a.w, b.w)
		&& FloatUtils::IsEqualF(a.h, b.h);
}

/**
* @brief Non-equality operator for SDL_FRects.
*
* @return 'true' if the first element is different than the second, 'false' if otherwise.
*/
inline bool operator!=(const SDL_FRect& a, const SDL_FRect& b)
{
	return !(a == b);
}

#endif // SDL_FRECTOPERATORS_H_
