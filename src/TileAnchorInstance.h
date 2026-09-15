#pragma once

#ifndef TILEANCHORINSTANCE_H_
#define TILEANCHORINSTANCE_H_

#include <SDL3/SDL_rect.h>

#include "ConstantConfiguration.h"

/**
* @brief Structure used to tell parts where their anchor is located and which tile they belong to.
*/
struct TileAnchorInstance
{
    SDL_Point anchorPosition = { 0, 0 };    /// The position of the anchor.
    int tileIndex = ConstantConfiguration::invalidTileIndex;    /// Index of the anchor.

    /**
    * @brief Constructor.
    * 
    * @param anchorX X position of the anchor.
    * @param anchorY Y position of the anchor.
    * @param tileIndex Index of the anchor.
    */
    TileAnchorInstance(int anchorX, int anchorY, int tileIndex);

    /**
    * @brief Constructor.
    *
    * @param anchorPosition Position of the anchor.
    * @param tileIndex Index of the anchor.
    */
    TileAnchorInstance(const SDL_Point& anchorPosition, int tileIndex);

    /**
    * @brief Is equal operator.
    */
    bool operator==(const TileAnchorInstance& b);
};

#endif // TILEANCHORINSTANCE_H_