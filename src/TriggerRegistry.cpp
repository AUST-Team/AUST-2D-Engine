#include <SDL3/SDL_log.h>

#include "TriggerRegistry.h"

#include "Trigger.h"
#include "Player.h"
#include "Map.h"

void TriggerRegistry::ProcessTriggers(MapUpdateFlags updateMask, Player* player)
{
    ++currentFrameID;

    SpatialBucket maxBucket = GetMaxBucketForUpdate(updateMask);

    /*
    * So basically, each trigger is categorized firstly by flag, then by spatial bucket.
    * Therefore, we first search by the flag bits, and for flag bit, we update the triggers within the spatial buckets given by the update.
    */

    ForEachFlag(updateMask, [&](MapUpdateFlags bit) {
        auto flagIt = triggersByFlag.find(bit);
        if (flagIt == triggersByFlag.end())
        {
            return;
        }

        for (size_t b = 0; b <= (+maxBucket); ++b)
        {
            const std::vector<Trigger*>& vec = flagIt->second[b];

            for (Trigger* t : vec)
            {
                if (!t || t->GetLastProcessedFrame() == currentFrameID)
                {
                    continue;
                }

                const SpatialBucket bucketType = static_cast<SpatialBucket>(b);

                if (!IsWithinBucketRange(t->GetPosition(), player->GetTilePosition(), bucketType))
                {
                    continue;
                }

                t->SetLastProcessedFrame(currentFrameID);
                t->TryFire(player, updateMask);
            }
        }
    });
}

bool TriggerRegistry::RegisterTrigger(Trigger* trigger)
{
    if (!trigger)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TriggerRegistry.RegisterTrigger: Attempted to register a null trigger.");
        return false;
    }

    const SpatialBucket bucket = trigger->GetSpatialBucket();
    const HashID id = trigger->GetTriggerId();

    auto [it, inserted] = triggersById.emplace(id, trigger);
    if (!inserted)
    {
        const std::string name = trigger->GetTriggerName();
        if (it->second->GetTriggerName() != name)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TriggerRegistry.RegisterTrigger: Trigger ID hash collision: '%s' vs '%s'", name.c_str(), it->second->GetTriggerName().c_str());
        }
        else
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TriggerRegistry.RegisterTrigger: Attempted to reigster a trigger with same name : '%s' vs '%s'", name.c_str(), it->second->GetTriggerName().c_str());
        }

        return false;
    }

    ForEachFlag(trigger->GetUpdateFlags(), [&](MapUpdateFlags bit) {
        for (size_t b = 0; b <= (+bucket); ++b)
        {
            triggersByFlag[bit][b].push_back(trigger);
        }
    });

    return true;
}

Trigger* TriggerRegistry::GetTriggerById(HashID triggerId)
{
    auto it = triggersById.find(triggerId);
    return it == triggersById.end() ? nullptr : it->second;
}

void TriggerRegistry::RemoveTrigger(Trigger* trigger)
{
    if (!trigger)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TriggerRegistry.RemoveTrigger: Attempted to remove a null trigger.");
        return;
    }

    const SpatialBucket bucket = trigger->GetSpatialBucket();

    ForEachFlag(trigger->GetUpdateFlags(), [&](MapUpdateFlags bit) {
        auto flagIt = triggersByFlag.find(bit);
        if (flagIt == triggersByFlag.end())
        {
            return;
        }

        for (size_t b = 0; b <= (+bucket); ++b)
        {
            std::erase(flagIt->second[b], trigger);
        }
    });

    triggersById.erase(trigger->GetTriggerId());
}

SpatialBucket TriggerRegistry::GetMaxBucketForUpdate(MapUpdateFlags updateMask)
{
    SpatialBucket maxBucket = SpatialBucket::TileOnly;

    if (HasFlag(updateMask, MapUpdateFlags::PlayerMoved) ||
        HasFlag(updateMask, MapUpdateFlags::PlayerDirectionChange))
    {
        return SpatialBucket::Far;
    }

    if (HasFlag(updateMask, MapUpdateFlags::Interact))
    {
        maxBucket = SpatialBucket::Near;
    }

    return maxBucket;
}
