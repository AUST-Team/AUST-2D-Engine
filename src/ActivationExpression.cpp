#include <SDL3/SDL_log.h>
#include <utility>

#include "ActivationExpression.h"

#include "Condition.h"
#include "CompositeCondition.h"
#include "MemoryTracker.h"

ActivationExpression::ActivationExpression(Condition* conditionRoot) :
    root(conditionRoot),
    updateFlags(MapUpdateFlags::None),
    spatialBucket(SpatialBucket::TileOnly)
{
    if (!root)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ActivationExpression.ActivationExpression: Passed an empty condition root.");
        return;
    }

    spatialBucket = GetSpatialBucketFromRadius(root->GetSpatialRadius());
    updateFlags = root->GetUpdateFlags();
}

ActivationExpression::ActivationExpression(ActivationExpression&& other) noexcept :
    root(std::exchange(other.root, nullptr)),
    updateFlags(other.updateFlags),
    spatialBucket(other.spatialBucket) {}

ActivationExpression::~ActivationExpression()
{
    if (root && !root->IsShared())
    {
		ENG_DELETE(root);
        root = nullptr;
    }
}

bool ActivationExpression::Evaluate(Trigger* trigger, Player* player, MapUpdateFlags updateMask) const
{
    if (!root)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ActivationExpression.Evaluate: Condition root is null. Defaulting to FALSE.");
        return false;
    }
    
    return root->IsSatisfied(trigger, player, updateMask);
}

MapUpdateFlags ActivationExpression::GetUpdateFlags() const 
{ 
    if (!root)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ActivationExpression.GetUpdateFlags: Attempted to get update flags of an null root. Defaulting to NONE.");
        return MapUpdateFlags::None;
    }

    return updateFlags;
}

SpatialBucket ActivationExpression::GetSpatialBucket() const
{
    if (!root)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ActivationExpression.GetUpdateFlags: Attempted to get update flags of an null root. Defaulting to TILE ONLY.");
        return SpatialBucket::TileOnly;
    }

    return spatialBucket;
}

ActivationExpression& ActivationExpression::operator=(ActivationExpression&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    root = std::exchange(other.root, nullptr);
    updateFlags = other.updateFlags;
    spatialBucket = other.spatialBucket;

    return *this;
}
