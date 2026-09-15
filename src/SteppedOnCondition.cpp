#include "SteppedOnCondition.h"

#include "Player.h"
#include "Trigger.h"
#include "SDL_PointOperators.h"

bool SteppedOnCondition::IsSatisfied(Trigger* trigger, Player* player, MapUpdateFlags updateMask) const
{
    if (!HasFlag(updateMask, SteppedOnCondition::GetUpdateFlags()))
    {
        return false;
    }

    return player->GetTilePosition() == trigger->GetPosition();
}

MapUpdateFlags SteppedOnCondition::GetUpdateFlags() const { return MapUpdateFlags::PlayerMoved; }

uint64_t SteppedOnCondition::GetSpatialRadius() const { return 0; }

bool SteppedOnCondition::IsShared() const { return Condition::IsShared(); }
