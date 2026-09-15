#pragma once

#ifndef RENDERABLE_H_
#define RENDERABLE_H_

#include <SDL3/SDL_rect.h>

#include "RenderLayer.h"
#include "FloatUtils.h"

struct SDL_Texture;

/**
* @brief Structure for a renderable object.
*/
struct Renderable
{
    SDL_Texture* texture = nullptr; /// The texture to render.
    SDL_FRect src = { 0, 0, 0, 0 }; /// The source rectangle.
    SDL_FRect dst = { 0, 0, 0, 0 }; /// The destination rectangle.
    float sortY = 0;    /// World-based depth for tiles.
    int zBias = 0;      /// Tie-breaker. User-defined.
    RenderLayer layer = RenderLayer::Ground;    /// The logical layer.
    Uint8 alpha = 255;  /// Alpha of the renderable.

    /**
    * @brief Less-than operator.
    */
    constexpr inline bool operator<(const Renderable& b)
    {
        if (this->layer != b.layer) 
        {
            return this->layer < b.layer;
        }

        if (!FloatUtils::IsEqualF(this->sortY, b.sortY))
        {
            return FloatUtils::IsLessF(this->sortY, b.sortY);
        }

        return this->zBias < b.zBias;
    }
};

#endif // RENDERABLE_H_