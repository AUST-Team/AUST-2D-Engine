#pragma once

#ifndef UICOLORCONFIGURATION_H_
#define UICOLORCONFIGURATION_H_

#include <SDL3/SDL_pixels.h>

/**
* @brief Configuration for UI colors.
*/
struct UIColorConfiguration
{
    SDL_Color background = { 50, 54, 62, 255 };    /// Background color.
    SDL_Color disabled = { 150, 150, 150, 255 };   /// Disabled button color.
};

#endif // UICOLORCONFIGURATION_H_