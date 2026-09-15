#include "ProximityCondition.h"

#include "Player.h"
#include "Trigger.h"

ProximityCondition::ProximityCondition(uint64_t range) :
	range(range) {}

bool ProximityCondition::IsSatisfied(Trigger* trigger, Player* player, MapUpdateFlags updateMask) const
{
    if (!HasFlag(updateMask, ProximityCondition::GetUpdateFlags()))
    {
        return false;
    }

    const SDL_Point tp = trigger->GetPosition();
    const SDL_Point pp = player->GetTilePosition();

    const int dx = std::abs(tp.x - pp.x);
    const int dy = std::abs(tp.y - pp.y);

    return (dx + dy) <= static_cast<int>(range);
}

MapUpdateFlags ProximityCondition::GetUpdateFlags() const { return MapUpdateFlags::PlayerMoved; }

uint64_t ProximityCondition::GetSpatialRadius() const { return range; }

bool ProximityCondition::IsShared() const { return Condition::IsShared(); }
