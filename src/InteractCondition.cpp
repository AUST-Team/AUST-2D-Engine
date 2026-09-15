#include <SDL3/SDL_log.h>

#include "InteractCondition.h"

#include "Trigger.h"
#include "Player.h"
#include "SDL_PointOperators.h"
#include "Configuration.h"

bool InteractCondition::IsSatisfied(Trigger* trigger, Player* player, MapUpdateFlags updateMask) const
{
    if (!HasFlag(updateMask, InteractCondition::GetUpdateFlags()))
    {
        return false;
    }

    if (player->IsMoving())
    {
        return false;
    }

    const SDL_Point tile = player->GetTilePosition();
    const Direction dir = player->GetLastDirection();

    if (IsSentinel(dir))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "InteractCondition.IsSatisfied: Attempted to interact but player has no facing direction.");
        return false;
    }

    const SDL_Point offset = GetDirectionOffset(dir);

    const SDL_Point target { tile.x + offset.x, tile.y + offset.y };
    return target == trigger->GetPosition();
}

MapUpdateFlags InteractCondition::GetUpdateFlags() const { return MapUpdateFlags::Interact; }

uint64_t InteractCondition::GetSpatialRadius() const { return Configuration::Get().conditions.interactRadius; }

bool InteractCondition::IsShared() const { return Condition::IsShared(); }
