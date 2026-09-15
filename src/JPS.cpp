#include <algorithm>
#include <SDL3/SDL_log.h>

#include "JPS.h"

#include "GameMap.h"
#include "Distances.h"

Path JPS::FindPath(const GameMap& map, int startX, int startY, int goalX, int goalY)
{
    if (!map.IsInMapBounds(startX, startY) || !map.IsInMapBounds(goalX, goalY))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "AStar.FindPath: Attempted to find a path to/from a point that was out of bounds.");
        return {};
    }

    nodePool.ResizeIfNeeded(map.GetMapWidth(), map.GetMapHeight());
    nodePool.BeginSearch();

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList;

    GridNode& startNode = nodePool.Get(startX, startY);
    startNode.g = 0;
    startNode.h = Distances::GetChebyshevDistanceTo(startX, startY, goalX, goalY);
    startNode.parentCoords = SDL_Point { startX, startY };
    startNode.openedStamp = nodePool.GetOpenStamp();

    openList.push({ startX, startY, startNode.g + startNode.h });

    while (!openList.empty())
    {
        Node current = openList.top();
        openList.pop();

        GridNode& currentNode = nodePool.Get(current.coords);
        currentNode.closedStamp = nodePool.GetClosedStamp();

        if (current.coords.x == goalX && current.coords.y == goalY)
        {
            Path path;
            int cx = goalX, cy = goalY;
            while (!(cx == startX && cy == startY))
            {
                path.nodes.emplace_back(cx, cy);
                GridNode& n = nodePool.Get(cx, cy);
                const int px = n.parentCoords.x;
                const int py = n.parentCoords.y;
                cx = px;
                cy = py;
            }
            std::reverse(path.nodes.begin(), path.nodes.end());
            return path;
        }

        IdentifySuccessors(map, current.coords.x, current.coords.y, goalX, goalY, openList);
    }

    return {};
}

std::optional<SDL_Point> JPS::Jump(const GameMap& map, int x, int y, int dx, int dy, int goalX, int goalY)
{
    const int nx = x + dx;
    const int ny = y + dy;

    // Out of bounds or solid tile.
    if (!map.IsInMapBounds(nx, ny) || map.IsTileSolid(nx, ny))
    {
        return std::nullopt;
    }

    // Goal reached.
    if (nx == goalX && ny == goalY)
    {
        return SDL_Point { nx, ny };
    }

    // Forced neighbors for cardinal directions.
    if (dx == 0 || dy == 0)
    {
        // Horizontal.
        if (dx != 0)
        {
            if ((map.IsInMapBounds(nx, ny - 1) && !map.IsTileSolid(nx, ny - 1) && map.IsTileSolid(nx - dx, ny - 1)) ||
                (map.IsInMapBounds(nx, ny + 1) && !map.IsTileSolid(nx, ny + 1) && map.IsTileSolid(nx - dx, ny + 1)))
            {
                return SDL_Point { nx, ny };
            }
        }
        else // Vertical.
        {
            if ((map.IsInMapBounds(nx - 1, ny) && !map.IsTileSolid(nx - 1, ny) && map.IsTileSolid(nx - 1, ny - dy)) ||
                (map.IsInMapBounds(nx + 1, ny) && !map.IsTileSolid(nx + 1, ny) && map.IsTileSolid(nx + 1, ny - dy)))
            {
                return SDL_Point { nx, ny };
            }
        }
    }
    else // Diagonal.
    {
        // Stop if blocked along either axis.
        if (map.IsTileSolid(nx - dx, ny) || map.IsTileSolid(nx, ny - dy))
        {
            return std::nullopt;
        }

        // Stop at forced neighbor along horizontal or vertical.
        if (Jump(map, nx, ny, dx, 0, goalX, goalY).has_value() ||
            Jump(map, nx, ny, 0, dy, goalX, goalY).has_value())
        {
            return SDL_Point { nx, ny };
        }
    }

    // Continue jumping in same direction.
    return Jump(map, nx, ny, dx, dy, goalX, goalY);
}

void JPS::IdentifySuccessors(const GameMap& map, int x, int y, int goalX, int goalY,
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>>& openList)
{
    const std::vector<SDL_Point> dirs = {
        SDL_Point {  0, -1 },
        SDL_Point {  0,  1 },
        SDL_Point { -1,  0 },
        SDL_Point {  1,  0 },
        SDL_Point { -1, -1 },
        SDL_Point {  1, -1 },
        SDL_Point { -1,  1 },
        SDL_Point {  1,  1 }
    };

    GridNode& current = nodePool.Get(x, y);

    for (const SDL_Point& dir : dirs)
    {
        std::optional<SDL_Point> jumpPoint = Jump(map, x, y, dir.x, dir.y, goalX, goalY);
        if (!jumpPoint.has_value())
        {
            continue;
        }

        int jx = jumpPoint->x;
        int jy = jumpPoint->y;

        GridNode& neighbor = nodePool.Get(jx, jy);
        int tentativeG = current.g + Distances::GetChebyshevDistanceTo(x, y, jx, jy);

        if (neighbor.openedStamp != nodePool.GetOpenStamp() || tentativeG < neighbor.g)
        {
            neighbor.g = tentativeG;
            neighbor.h = Distances::GetChebyshevDistanceTo(jx, jy, goalX, goalY);
            neighbor.parentCoords = SDL_Point { x, y };

            if (neighbor.openedStamp != nodePool.GetOpenStamp())
            {
                neighbor.openedStamp = nodePool.GetOpenStamp();
                openList.push({ jx, jy, neighbor.g + neighbor.h });
            }
        }
    }
}

