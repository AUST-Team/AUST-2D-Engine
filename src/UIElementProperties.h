#pragma once

#ifndef UIELEMENTPROPERTIES_H_
#define UIELEMENTPROPERTIES_H_

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_pixels.h>

/**
* @brief A structure separate from UIElement so you can have move the propeties without instancing an UIElement.
*/
struct UIElementProperties
{
    SDL_FRect bounds = { 0.0f, 0.0f, 0.0f, 0.0f };          /// Bounds of the element.
    SDL_Color backgroundColor = { 0, 0, 0, 0 };             /// Background color of the element.
    SDL_Color borderColor = { 255, 255, 255, 255 };         /// Border color of the element.
};

#endif // UIELEMENTPROPERTIES_H_