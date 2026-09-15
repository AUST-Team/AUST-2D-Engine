#include <SDL3/SDL.h>

#include "GuardPathStrategy.h"
#include "GameMap.h"
#include "Pathfinding.h"
#include "Guard.h"

GuardPathStrategy::GuardPathStrategy(const SDL_Point& destination, PathStrategy strategy) :
    target(destination), 
    pathfindingMethod(strategy) {}

GuardPathStrategy::GuardPathStrategy(int targetX, int targetY, PathStrategy strategy) :
    GuardPathStrategy(SDL_Point { targetX, targetY }, strategy) {}

Direction GuardPathStrategy::GetDirection(Guard& guard)
{
    const GameMap& map = GameMap::GetInstance();
    const int sx = guard.GetTileX();
    const int sy = guard.GetTileY();
    const int gx = target.x;
    const int gy = target.y;

    // If already there.
    if (sx == gx && sy == gy)
    {
        reachedPointFlag = true;
        return GetSentinel<Direction>();
    }

    // Need a path?
    if (path.Empty())
    {
        path = Pathfinding::GetInstance().FindPath(map, sx, sy, gx, gy, pathfindingMethod);
        currentIndex = 0;

        if (path.Empty())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GuardPathStrategy: No path found");
            reachedPointFlag = true;
            return GetSentinel<Direction>();
        }
    }

    // Safety: ensure index valid.
    if (currentIndex >= path.nodes.size())
    {
        reachedPointFlag = true;
        return GetSentinel<Direction>();
    }

    SDL_Point nextPoint = path.nodes[currentIndex];

    // If we have arrived *after movement* in previous frame.
    if (sx == nextPoint.x && sy == nextPoint.y)
    {
        currentIndex++;

        if (currentIndex >= path.nodes.size())
        {
            reachedPointFlag = true;
            return GetSentinel<Direction>();
        }

        nextPoint = path.nodes[currentIndex];
    }

    // Compute step direction (tile-based).
    int dx = nextPoint.x - sx;
    int dy = nextPoint.y - sy;

    // Normalise to -1,0,1.
    if (dx != 0) dx = (dx > 0 ? 1 : -1);
    if (dy != 0) dy = (dy > 0 ? 1 : -1);

    // Ensure move is valid.
    if (map.IsTileSolid(sx + dx, sy + dy))
    {
        // Path invalidated.
        ForcePathReconstruction();
        return GetSentinel<Direction>();
    }

    if (dx == 1)  return Direction::Right;
    if (dx == -1) return Direction::Left;
    if (dy == 1)  return Direction::Down;
    if (dy == -1) return Direction::Up;

    return GetSentinel<Direction>();
}

void GuardPathStrategy::Update(double deltaTime) { GuardStrategy::Update(deltaTime); }
void GuardPathStrategy::SetTarget(int x, int y) { SetTarget(SDL_Point { x, y }); }

void GuardPathStrategy::SetTarget(const SDL_Point& newTarget) 
{ 
    target = newTarget; 
    currentIndex = 0; 
    path.Clear(); 
    reachedPointFlag = false;
}

bool GuardPathStrategy::HasReachedPoint() const { return reachedPointFlag; }

void GuardPathStrategy::ResetReachedPointFlag() { reachedPointFlag = false; }

void GuardPathStrategy::ForcePathReconstruction()
{
    currentIndex = 0;
    path.Clear();
    reachedPointFlag = false;
}
