#include <SDL3/SDL_log.h>

#include "AnimationSystem.h"

#include "AnimationClip.h"
#include "TileInstance.h"
#include "AnimationState.h"

AnimationState* AnimationSystem::GetOrCreate(TileInstance* tile, TileAnimationType type)
{
    if (!tile)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AnimationSystem.GetOrCreate: Passed null tile.");
        return nullptr;
    }

    if (!HasFlag(tile->animationMask, type))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AnimationSystem.GetOrCreate: Tile doesn't contain animation type wanted.");
        return nullptr;
    }

    std::unordered_map<TileAnimationType, AnimationState>& animMap = states[tile];
    auto it = animMap.find(type);
    if (it != animMap.end())
    {
        it->second.EnsureUpToDate(globalAnimationTime, tile->animationTable[TileReactionIndexFromType(type)]);
        return &it->second;
    }

    auto [insertIt, inserted] = animMap.emplace(type, AnimationState{});
    return &insertIt->second;
}

void AnimationSystem::Update(double deltaTime)
{
    globalAnimationTime += static_cast<float>(deltaTime);
    //for (auto& [tile, animMap] : states)
    //{
    //    if (!tile)
    //    {
    //        continue;
    //    }

    //    for (auto it = animMap.begin(); it != animMap.end(); )
    //    {
    //        AnimationType type = it->first;
    //        AnimationState& state = it->second;

    //        //auto clipIt = tile->animations.find(type);
    //        //if (clipIt == tile->animations.end())
    //        //{
    //        //    // Tile lost animation (might make hot reloads).
    //        //    it = animMap.erase(it);
    //        //    continue;
    //        //}

    //        //state.Advance(deltaTime, clipIt->second);
    //        ++it;
    //    }
    //}
}

float AnimationSystem::ComputeTilePhaseOffset(int tileX, int tileY, float clipDuration)
{
    uint64_t hash = (uint64_t(tileX) << 32) | uint32_t(tileY);
    hash ^= (hash >> 33);
    hash *= 0xff51afd7ed558ccd;
    hash ^= (hash >> 33);
    hash *= 0xc4ceb9fe1a85ec53;
    hash ^= (hash >> 33);

    const double phase01 = static_cast<double>(hash >> 11) * (1.0 / 9007199254740992.0);

    return static_cast<float>(phase01 * clipDuration);
}

bool AnimationSystem::IsAnimationFinished(TileInstance* tile, TileAnimationType type, float startTime) const
{
    if (!HasFlag(tile->animationMask, type))
    {
        return true;
    }

    AnimationClip* clip = &tile->animationTable[TileReactionIndexFromType(type)];

    const float elapsedTime = GetGlobalAnimationTime() - startTime;
    return elapsedTime >= clip->GetTotalDuration();
}

float AnimationSystem::GetGlobalAnimationTime() const { return globalAnimationTime; }

size_t AnimationSystem::GetCachedFrame(const AnimationClip& clip, float t)
{
    AnimationFrameCache& entry = frameCache[&clip];
    if (entry.time != t)
    {
        entry.frameIndex = clip.GetFrameAtTime(t);
        entry.time = t;
    }
    return entry.frameIndex;
}

void AnimationSystem::BeginFrame() { frameCache.clear(); }
