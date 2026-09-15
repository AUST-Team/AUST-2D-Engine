#include "Pathfinding.h"

#include "Guard.h"
#include "Player.h"

Path Pathfinding::FindPath(int startX, int startY, int goalX, int goalY, PathStrategy strategy)
{
    return FindPath(GameMap::GetInstance(), startX, startY, goalX, goalY, strategy);
}

Path Pathfinding::FindPath(const GameMap& map, int startX, int startY, int goalX, int goalY, PathStrategy strategy)
{
    switch (strategy)
    {
        case PathStrategy::AStar:   return astar.FindPath(map, startX, startY, goalX, goalY);
        case PathStrategy::JPS:     return jps.FindPath(map, startX, startY, goalX, goalY);
        default:                    return astar.FindPath(map, startX, startY, goalX, goalY);
    }
}

Direction Pathfinding::FindNextDirectionForGuard(Guard& guard, const Player* player, PathStrategy strategy)
{
    if (!player)
    {
		return GetSentinel<Direction>();
    }

    const GameMap& map = GameMap::GetInstance();

    const int sx = guard.GetTileX();
    const int sy = guard.GetTileY();
    const int gx = player->GetTileX();
    const int gy = player->GetTileY();

    Path path = FindPath(map, sx, sy, gx, gy, strategy);

    if (path.Empty())
    {
        return GetSentinel<Direction>();
    }

    const SDL_Point& next = path.nodes.front();

    int dx = next.x - sx;
    int dy = next.y - sy;

    if (dx != 0) dx = (dx > 0 ? 1 : -1);
    if (dy != 0) dy = (dy > 0 ? 1 : -1);

    if (dx == 1)  return Direction::Right;
    if (dx == -1) return Direction::Left;
    if (dy == 1)  return Direction::Down;
    if (dy == -1) return Direction::Up;

    return GetSentinel<Direction>();
}