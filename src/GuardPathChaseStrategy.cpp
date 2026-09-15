#include <SDL3/SDL_log.h>

#include "GuardPathChaseStrategy.h"

#include "Player.h"
#include "Guard.h"
#include "GameMap.h"

GuardPathChaseStrategy::GuardPathChaseStrategy(PathStrategy strategy, int thresholdTiles) :
    GuardPathStrategy({ 0,0 }, strategy), recalcDistanceThreshold(thresholdTiles) {}

Direction GuardPathChaseStrategy::GetDirection(Guard& guard)
{
    const Player* player = Player::GetInstance();
    if (!player)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GuardPathChaseStrategy.GetDirection: Somehow, the player pointer is null.");
        return GetSentinel<Direction>();
    }

    const int sx = guard.GetTileX();
    const int sy = guard.GetTileY();
    const int gx = player->GetTileX();
    const int gy = player->GetTileY();

    if ((std::abs(sx - gx) == 0 && std::abs(sy - gy) == 1) || (std::abs(sy - gy) == 0 && std::abs(sx - gx) == 1))
    {
		return GetSentinel<Direction>();
    }

    bool recalc = false;

    if (HasReachedPoint() || path.Empty())
    {
        recalc = true;
    }
    else
    {
        // Check distance from player's current tile to last node in path
        const SDL_Point& last = path.nodes.back();
        if (std::abs(last.x - gx) > recalcDistanceThreshold ||
            std::abs(last.y - gy) > recalcDistanceThreshold)
        {
            recalc = true;
        }
        else
        {
            // Check if player is closer than the next path node
            const SDL_Point next = path.nodes[currentIndex];
            const int distNext = std::abs(next.x - sx) + std::abs(next.y - sy);
            const int distPlayer = std::abs(gx - sx) + std::abs(gy - sy);

            if (distPlayer < distNext)
            {
                recalc = true;
            }
        }
    }

    if (recalc)
    {
        SetTarget(SDL_Point { gx, gy });
    }

    return GuardPathStrategy::GetDirection(guard);
}
