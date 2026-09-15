#pragma once

#ifndef ANIMATIONFRAMECACHE_H_
#define ANIMATIONFRAMECACHE_H_

/**
* @brief Structure for caching an animation frame.
*/
struct AnimationFrameCache
{
    float time = -1.0f;     /// The time this frame is found at.
    size_t frameIndex = 0;  /// The index of the frame in the clip.
};

#endif // ANIMATIONFRAMECACHE_H_