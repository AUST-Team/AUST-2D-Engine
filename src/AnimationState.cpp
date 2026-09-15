#include <SDL3/SDL_log.h>

#include "AnimationState.h"

#include "AnimationClip.h"
#include "FloatUtils.h"

void AnimationState::Advance(double deltaTime, const AnimationClip& clip)
{
    if (clip.frames.empty())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "AnimationState.Advance: Clip has no frames.");
        return;
    }

    time += static_cast<float>(deltaTime);
    elapsedTime += static_cast<float>(deltaTime);

    while (time >= clip.frames[frameIndex].duration)
    {
        time -= clip.frames[frameIndex].duration;
        frameIndex++;

        if (frameIndex >= clip.frames.size())
        {
            frameIndex = clip.loopAnimationFlag ? 0 : clip.frames.size() - 1;
        }
    }
}

void AnimationState::EnsureUpToDate(float globalTime, const AnimationClip& clip)
{
    if (lastUpdateTime == globalTime)
    {
        return;
    }

    float delta = globalTime - lastUpdateTime;
    lastUpdateTime = globalTime;

    if (clip.frames.empty())
    {
        return;
    }

    elapsedTime += delta;
    time += delta;

    while (time >= clip.frames[frameIndex].duration)
    {
        time -= clip.frames[frameIndex].duration;
        frameIndex++;

        if (frameIndex >= clip.frames.size())
        {
            frameIndex = clip.loopAnimationFlag ? 0 : clip.frames.size() - 1;
        }
    }
}

bool AnimationState::IsFinished(const AnimationClip& clip) const { return elapsedTime >= clip.GetTotalDuration() || clip.frames.empty(); }
