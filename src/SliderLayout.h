#pragma once

#ifndef SLIDERLAYOUT_H_
#define SLIDERLAYOUT_H_

#include <SDL3/SDL_rect.h>

/**
* @brief Wrapper structure over a slider layout.
*/
struct SliderLayout
{
    SDL_FRect track;        /// Rectangle of the full track.
    SDL_FRect activeTrack;  /// Rectangle of the active track.
    SDL_FRect handle;       /// Rectangle of the handle.
    SDL_FRect fullBounds;   /// Rectangle of the full bounds of the text field + slider.
};

#endif // SLIDERLAYOUT_H_