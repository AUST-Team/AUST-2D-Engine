#pragma once

#ifndef SEMIANIMATIONOVERRIDE_H_
#define SEMIANIMATIONOVERRIDE_H_

#include "TileReactionType.h"
#include "ConstantConfiguration.h"

/**
* @brief Structure for a semi-animation override.
* 
* To be used for when tiles need a different animation from the default one.
* 
* Uses the same animation state as the global one!
*/
struct SemiAnimationOverride
{
    int tileIndex = ConstantConfiguration::invalidTileIndex;  /// Index of the tile. DEFAULT_INVALID_TILE_INDEX for any tile on that cell.
    float startTime = 0.0f;     /// The start time for the animation. Used to remove the animation should the oneShotFlag be set.
    bool oneShotFlag = false;   /// Flag if the animation should only play once.
    TileAnimationType type = TileAnimationType::None; /// Animation type.

    /**
    * @brief Default constructor.
    */
    SemiAnimationOverride() = default;

    /**
    * @brief Constructor.
    * 
    * @param tileIndex Index of the tile.
    * @param type The animation type.
    * @param startTime The start time of the animation, in relation to the global animation time.
    * @param oneShot If the animation should only play once or not.
    */
    SemiAnimationOverride(int tileIndex, TileAnimationType type, float startTime = 0.0f, bool oneShot = true);
};

#endif // SEMIANIMATIONOVERRIDE_H_