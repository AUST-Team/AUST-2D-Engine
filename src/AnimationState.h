#pragma once

#ifndef ANIMATIONSTATE_H_
#define ANIMATIONSTATE_H_

struct AnimationClip;

/**
* @brief Structure for an animation state.
*/
struct AnimationState
{
    float elapsedTime = 0.0f;           /// Total elapsed time.
    float time = 0.0f;                  /// Timer for the duration.
    float lastUpdateTime = 0.0f;        /// The moment of the last update.
    size_t frameIndex = 0;              /// Index of the frame.

    /**
    * @brief Advances an animation clip.
    * 
    * @param deltaTime The delta time of the main app loop.
    * @param clip Reference to the clip.
    */
    void Advance(double deltaTime, const AnimationClip& clip);

    /**
    * @brief Ensures an animation state (clip) is up to date.
    * 
    * @param globalTime The global animation time.
    * @param clip Reference to the clip.
    */
    void EnsureUpToDate(float globalTime, const AnimationClip& clip);

    /**
    * @brief Checks if the animation clip has finished.
    * 
    * For loopabale animations, it checks if it has done atleast 1 full loop.
    * 
    * @return 'true' if it has finished, 'false' if otherwise.
    */
    bool IsFinished(const AnimationClip& clip) const;
};

#endif // ANIMATIONSTATE_H_