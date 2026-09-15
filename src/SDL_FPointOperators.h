#pragma once

#ifndef SDL_FPOINTOPERATORS_H_
#define SDL_FPOINTOPERATORS_H_

#include <SDL3/SDL_rect.h>

#include "FloatUtils.h"

/**
* @brief Equality operator for SDL_FPoint
*
* @param a SDL_FPoint to compare
* @param b SDL_FPoint to compare
*
* @return true if 'a == b', false otherwise.
*/
inline bool operator==(const SDL_FPoint& a, const SDL_FPoint& b)
{
    return (FloatUtils::IsEqualF(a.x, b.x) &&
            FloatUtils::IsEqualF(a.y, b.y));
}

/**
* @brief Not-equal operator for SDL_FPoint
*
* @param a SDL_FPoint to compare
* @param b SDL_FPoint to compare
*
* @return true if 'a != b', false otherwise.
*/
inline bool operator!=(const SDL_FPoint& a, const SDL_FPoint& b)
{
    return !(a == b);
}


#endif // SDL_FPOINTOPERATORS_H_