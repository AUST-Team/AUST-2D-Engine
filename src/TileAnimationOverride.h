#pragma once

#ifndef TILEANIMATIONOVERRIDE_H_
#define TILEANIMATIONOVERRIDE_H_

#include "SemiAnimationOverride.h"
#include "FullAnimationOverride.h"
#include "SDL_PointOperators.h"

/**
* @brief Structure for tile animation overrides.
*
* Full overrides have precedence over semi overrides.
* 
* Map of [AnchorPoint, map[TileIndex, AnimationOverride]]
*/
struct TileAnimationOverride
{
    std::unordered_map<SDL_Point, std::unordered_map<int, SemiAnimationOverride>> semiOverrides;    /// Semi overrides.
    std::unordered_map<SDL_Point, std::unordered_map<int, FullAnimationOverride>> fullOverrides;    /// Full overrides.
};

#endif // TILEANIMATIONOVERRIDE_H_