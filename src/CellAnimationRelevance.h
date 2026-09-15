#pragma once

#ifndef CELLANIMATIONRELEVANCE_H_
#define CELLANIMATIONRELEVANCE_H_

#include <vector>

#include "TileReactionType.h"

/**
* @brief Structure for holding a vector of tiles with the same animation.
*/
struct CellAnimationRelevance
{
    TileAnimationType supportedAnimations = TileAnimationType::None;  /// Mask of the supported animation(s).
    std::vector<int> tileIndices;   /// Vector of tile indexes on a particular cell that have that type of animation and it isn't the default animation.
};

#endif // CELLANIMATIONRELEVANCE_H_