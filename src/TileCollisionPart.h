#pragma once

#ifndef TILECOLLISIONPART_H_
#define TILECOLLISIONPART_H_

#include <SDL3/SDL_rect.h>

/**
* @brief Structure for a tile collision part.
*/
struct TileCollisionPart
{
    SDL_Point offset = { 0, 0 };    /// Offset of of the tile part.
    bool solidFlag = false;         /// Flag if the tile part is solid.
    bool blocksVisionFlag = false;  /// Flag if the tile part blocks vision.
};

#endif //  TILECOLLISIONPART_H_