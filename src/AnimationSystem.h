#pragma once

#ifndef ANIMATIONSYSTEM_H_
#define ANIMATIONSYSTEM_H_

#include <unordered_map>

#include "FNV1aHash.h"
#include "TileReactionType.h"
#include "AnimationState.h"
#include "AnimationFrameCache.h"

struct TileInstance;
struct AnimationClip;

/**
* @brief Animation system, mostly usef for tiles.
*/
class AnimationSystem
{
private:
    std::unordered_map<const AnimationClip*, AnimationFrameCache> frameCache;   /// Map of cached animation frames.

    std::unordered_map< 
        TileInstance*,
        std::unordered_map<
        TileAnimationType,
        AnimationState
        >
    > states; /// Stores the animation states of a certain tile for all their animation types. NON OWNING POINTERS!
    float globalAnimationTime = 0.0f;  /// Time since the start of the system. In milliseconds.

public:

    /**
    * @brief Returns or creates and returns a animation state for a tile and animation type.
    * 
    * @param tile Pointer to the tile type.
    * @param type The animation type.
    * 
    * @return Pointer to the animation state of the respective type of the respective tile.
    */
    AnimationState* GetOrCreate(TileInstance* tile, TileAnimationType type);

    /**
    * @brief Updates the animation system.
    * 
    * @param deltaTime The delta time of the main game loop.
    */
    void Update(double deltaTime);

    /**
    * @brief Calculates a deterministic phase offset (using fmix64)
    * 
    * @param tileX The X coordinate of the tile.
    * @param tileY The Y coordinate of the tile.
    * @param clipDuration The duration of the clip in milliseconds.
    * 
    * @return The pseudo-random phase offset (value between 0 and 1) of the tile.
    */
    float ComputeTilePhaseOffset(int tileX, int tileY, float clipDuration);

    /**
    * @brief Checks if a particular animation is finished.
    * 
    * @param tile Pointer to the tile.
    * @param type The animation type.
    * @param startTime The time the animation started.
    * 
    * @return 'true' if the animation is finished, 'false' otherwise.
    */
    bool IsAnimationFinished(TileInstance* tile, TileAnimationType type, float startTime) const;

    /**
    * @brief Returns the global animation time.
    * 
    * @return The global animation time.
    */
    float GetGlobalAnimationTime() const;

    /**
    * @brief Returns the cached frame of a clip at a certain time.
    * 
    * @param clip Reference to the clip.
    * @param t Moment of the frame.
    * 
    * @return The index of the frame in the clip.
    */
    size_t GetCachedFrame(const AnimationClip& clip, float t);

    /**
    * @brief Begins a new frame.
    */
    void BeginFrame();
};

#endif // ANIMATIONSYSTEM_H_