#pragma once

#ifndef ANIMATIONCLIP_H_
#define ANIMATIUONCLIP_H_

#include <vector>

#include "AnimationFrame.h"

/**
* @brief Animation clip. Holds a number of animation frames.
*/
struct AnimationClip
{
    std::vector<AnimationFrame> frames; /// Vector of frames.
    bool loopAnimationFlag = true;  /// Flag if the animation should be looped or not.
    bool randomOffsetFlag = false;  /// Flag if the animation should have a random offset applied.

    /**
    * @brief Returns the animation at a given moment.
    * 
    * @param t The moment when the animation frame should play.
    * 
    * @return The index of the animation frame at that respective moment.
    */
    size_t GetFrameAtTime(float t) const;

    /**
    * @brief Returns the total duration of the clip.
    *
    * @return The total duration of the clip.
    */
    float GetTotalDuration() const;
};

#endif // ANIMATIONCLIP_H_