#pragma once

#ifndef SDL_COLOROPERATORS_H_
#define SDL_COLOROPERATORS_H_

#include <SDL3/SDL_pixels.h>

/**
* @brief Equality operator for SDL_Color
*
* @param a SDL_Color to compare
* @param b SDL_Color to compare
*
* @return true if 'a == b', false otherwise.
*/
constexpr inline bool operator==(const SDL_Color& a, const SDL_Color& b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

/**
* @brief Inequality operator for SDL_Color
*
* @param a SDL_Color to compare
* @param b SDL_Color to compare
*
* @return true if 'a != b', false otherwise.
*/
constexpr inline bool operator!=(const SDL_Color& a, const SDL_Color& b)
{
    return !(a == b);
}

#endif // SDL_COLOROPERATORS_H_