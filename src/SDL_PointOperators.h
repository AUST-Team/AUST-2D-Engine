#pragma once

#ifndef SDL_POINTOPERATORS_H_
#define SDL_POINTOPERATORS_H_

#include <SDL3/SDL_rect.h>
#include <functional>

/**
* @brief Equality operator for SDL_Point
*
* @param a SDL_Point to compare
* @param b SDL_Point to compare
*
* @return true if 'a == b', false otherwise.
*/
constexpr inline bool operator==(const SDL_Point& a, const SDL_Point& b)
{
    return ((a.x == b.x) && (a.y == b.y));
}

/**
* @brief Not-equal operator for SDL_Point
*
* @param a SDL_Point to compare
* @param b SDL_Point to compare
*
* @return true if 'a != b', false otherwise.
*/
constexpr inline bool operator!=(const SDL_Point& a, const SDL_Point& b)
{
    return !(a == b);
}

// Tells the compiler how to hash a SDL_Point.
namespace std
{
    template <>
    struct hash<SDL_Point>
    {
        size_t operator()(const SDL_Point& p) const noexcept
        {
            return (static_cast<size_t>(static_cast<uint32_t>(p.x)) << 32) |
                static_cast<uint32_t>(p.y);
        }
    };

    template <>
    struct equal_to<SDL_Point>
    {
        bool operator()(const SDL_Point& lhs, const SDL_Point& rhs) const noexcept
        {
            return lhs.x == rhs.x && lhs.y == rhs.y;
        }
    };
}

#endif // SDL_POINTOPERATORS_H_