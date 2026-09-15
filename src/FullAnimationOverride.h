#pragma once

#ifndef FULLANIMATIONOVERRIDE_H_
#define FULLANIMATIONOVERRIDE_H_

#include "ConstantConfiguration.h"
#include "TileReactionType.h"
#include "AnimationState.h"
#include "AnimationClip.h"

/**
* @brief A full animation override.
* 
* Tiles with this will be given their own animation state! Will take precendence over SemiAnimationOverrides!
* 
* Recommend to use with the oneShotFlag set to 'true'.
*/
struct FullAnimationOverride
{
    AnimationState state;   /// The state of the animation.
    AnimationClip clip;     /// The clip of the animation. It is a copy of the clip from the tile index given with the animation given.
    int tileIndex = ConstantConfiguration::invalidTileIndex;  /// Index of the tile. DEFAULT_INVALID_TILE_INDEX for any tile on that cell.
    TileAnimationType type = TileAnimationType::None; /// Animation type.
    bool oneShotFlag = true;   /// Flag if the animation should only play once. If set, will be removed once the animation finishes.

    /**
    * @brief Default constructor.
    */
    FullAnimationOverride() = default;

    /**
    * Constructor.
    * 
    * @param tileIndex Index of the tile.
    * @param type The animation type.
    * @param oneShot If the animation should only play once or not.
    */
    FullAnimationOverride(int tileIndex, TileAnimationType type, bool oneShot = true);
};

#endif // FULLANIMATIONOVERRIDE_H_