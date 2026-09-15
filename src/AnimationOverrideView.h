#pragma once

#ifndef ANIMATIONOVERRIDEVIEW_H_
#define ANIMATIONOVERRIDEVIEW_H_

struct SemiAnimationOverride;
struct FullAnimationOverride;

/**
* @brief Structure for an animation override view.
*/
struct AnimationOverrideView
{
    SemiAnimationOverride* semi = nullptr;    /// Pointer to the semi animation override. NON OWNING!
    FullAnimationOverride* full = nullptr;    /// Pointer to the semi animation override. NON OWNING!

    /**
    * @brief Checks if the view has an override.
    * 
    * @return 'true' if it does, 'false' otherwise.
    */
    bool HasOverride() const;
};

#endif // ANIMATIONOVERRIDEVIEW_H_