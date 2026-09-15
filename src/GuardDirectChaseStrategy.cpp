#include "GuardDirectChaseStrategy.h"

#include "Guard.h"
#include "Player.h"
#include "Configuration.h"
#include "FloatUtils.h"
#include "GameMap.h"
#include "Pathfinding.h"

Direction GuardDirectChaseStrategy::GetDirection(Guard& guard)
{
    const Player* player = Player::GetInstance();

    const float dx = player->GetX() - guard.GetX();
    const float dy = player->GetY() - guard.GetY();

    const GameConfiguration& config = Configuration::Get();
    float tileWidth = static_cast<float>(config.tiles.width);
    float tileHeight = static_cast<float>(config.tiles.height);

    // If the player is one tile away, stop the movement.
    if ((FloatUtils::IsZeroF(dx) && FloatUtils::IsEqualAbsF(dy, tileHeight)) || (FloatUtils::IsZeroF(dy) && FloatUtils::IsEqualAbsF(dx, tileWidth)))
    {
        return GetSentinel<Direction>();
    }

    return GetDirectionTo(dx, dy);
}

void GuardDirectChaseStrategy::Update(double deltaTime) { GuardStrategy::Update(deltaTime); }
