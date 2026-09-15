#pragma once

#ifndef ANIMATIONFRAME_H_
#define ANIMATIONFRAME_H_

#include <SDL3/SDL_rect.h>

/**
* @brief Strucrture for an animation frame.
*/
struct AnimationFrame
{
    SDL_FRect src = { 0.0f, 0.0f, 0.0f, 0.0f };  /// Source tile of the frame.
    float duration = 1.0f; /// Duration in seconds.
};

#endif // ANIMATIONFRAME_H_
