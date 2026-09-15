#pragma once

#ifndef TILERENDERPART_H_
#define TILERENDERPART_H_

#include <SDL3/SDL_rect.h>
#include <array>

#include "RenderLayer.h"
#include "AnimationClip.h"
#include "TileReactionType.h"

/**
* @brief Structure for a part of a tile. 1-part tiles use this too.
*/
struct TileRenderPart
{
    SDL_FRect srcRect = { 0.0f , 0.0f, 0.0f, 0.0f };    /// Rectangle (floating point) of the position of the tile in the sprite sheet.
                                                        /// Only used if there is no animation, or if the animation is defined poorly.
    SDL_Point offset = { 0, 0 };    /// Offset of the tile part (used for multi-part tiles).
    int zBias = 0;  /// Fine depth adjustment.
    TileAnimationType defaultAnimation = TileAnimationType::None;  /// The default animation, if any.
    TileAnimationType allowedAnimationsMask = TileAnimationType::None;  /// Mask of allowed animations.
    RenderLayer layer = RenderLayer::Ground;  /// Logical render layer.
};

#endif // TILERENDERPART_H_