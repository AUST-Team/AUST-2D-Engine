#include <cmath>

#include "AnimationClip.h"

size_t AnimationClip::GetFrameAtTime(float t) const
{
    if (frames.empty())
    {
        return 0;
    }

    if (loopAnimationFlag) 
    {
        t = std::fmod(t, GetTotalDuration());
    }

    float accum = 0;
    for (size_t i = 0; i < frames.size(); ++i)
    {
        accum += frames[i].duration;
        if (t < accum)
        {
            return i;
        }
    }
    return frames.size() - 1;
}

float AnimationClip::GetTotalDuration() const
{
    float total = 0;
    for (const AnimationFrame& f : frames)
    {
        total += f.duration;
    }
    return total;
}