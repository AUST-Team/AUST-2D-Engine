#include <SDL3/SDL_log.h>

#include "FacingCondition.h"

#include "Player.h"
#include "Trigger.h"
#include "Configuration.h"

FacingCondition::FacingCondition(FacingConditionType type) :
    facingType(type) {}

bool FacingCondition::IsSatisfied(Trigger* trigger, Player* player, MapUpdateFlags updateMask) const
{
    if (!HasFlag(updateMask, FacingCondition::GetUpdateFlags()))
    {
        return false;
    }

    Direction dir = player->GetLastDirection();
    if (IsSentinel(dir))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FacingCondition.IsSatisfied: Attempted to check condition, but player has no direction.");
        return false;
    }

    const SDL_Point playerPos = player->GetTilePosition();
    const SDL_Point triggerPos = trigger->GetPosition();

    const int dx = triggerPos.x - playerPos.x;
    const int dy = triggerPos.y - playerPos.y;

    Direction triggerDir = GetDirectionTo(
        static_cast<float>(dx), 
        static_cast<float>(dy)
    );

    if (IsSentinel(triggerDir))
    {
        return false;
    }

    if (facingType == FacingConditionType::LookAt)
    {
        return dir == triggerDir;
    }
    else
    {
        return dir != triggerDir;
    }

}

MapUpdateFlags FacingCondition::GetUpdateFlags() const { return MapUpdateFlags::PlayerDirectionChange; }

uint64_t FacingCondition::GetSpatialRadius() const { return Configuration::Get().conditions.facingRadius; }

bool FacingCondition::IsShared() const { return Condition::IsShared(); }
