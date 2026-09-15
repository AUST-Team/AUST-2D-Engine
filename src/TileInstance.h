#pragma once

#ifndef TILETYPE_H_
#define TILETYPE_H_

#include <SDL3/SDL_rect.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "FNV1aHash.h"
#include "TileRenderPart.h"
#include "TileCollisionPart.h"
#include "Size.h"
#include "TilePlacementMode.h"
#include "TileMetadataPart.h"
#include "TileAudio.h"

struct SDL_Renderer;

/*
* So a TileInstance is defined by an id and name (and index inside TileRegistry).
* A TileInstance can be made out of multiple parts; render and collision parts.
*   - TileCollisionPart is used for collisions. Can have collisions or block vision by part.
*   - TileRenderPart is used for rendering. A TileRenderPart can have an animation (per part!).
*       This animation contains a vector of AnimationFrames. If the TileRenderPart does not have an animation, it uses the srcRect to be drawn.
*       If the TileRenderPart does contain an animation, srcRect is not used, but instead the srcRects inside each AnimationFrames.
* 
* Got it? Glad.
*/

/**
* @brief Structure containing the information about a tile.
*/
struct TileInstance
{
    HashID id = 0;      /// ID of the tile. Rendering uses the tile index!
    Size footprint = { 0, 0 };  /// The footprint of the tile.
    std::string name;   /// Name of the tile. Rendering / maps use the tile index behind the scenes.
    std::vector<TileCollisionPart> collisionParts;  /// Vector of collision parts of the tile.
    std::vector<TileRenderPart> renderParts;        /// Vector of renderable parts of the tile.
    std::vector<TileMetadataPart> metadataParts;    /// Vector of metadata parts of the tile.
    std::array<AnimationClip, TileReactionCount> animationTable;  /// Array of animations.
    std::array<TileAudio, TileReactionCount> audioTable;          /// Array of sounds a tile can have.
    bool hasNonDefaultAnimations = false;   /// Flag if any of the render parts have non default animations.
    bool hasNonDefaultSounds = false;       /// Flag if any of the metadata parts have non default sounds.
    TileSoundType soundMask = TileSoundType::None;  /// Mask for sound types for fast look-up.
    TileAnimationType animationMask = TileAnimationType::None;  /// Mask for animation types for fast lookup.
    TilePlacementMode placementMode = TilePlacementMode::Replace;   /// Placement mode of the tile.
};

#endif // TILETYPE_H_
