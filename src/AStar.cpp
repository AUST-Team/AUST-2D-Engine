#include <queue>
#include <SDL3/SDL_log.h>

#include "AStar.h"

#include "GameMap.h"
#include "Distances.h"
#include "Node.h"

Path AStar::FindPath(const GameMap& map, int startX, int startY, int goalX, int goalY)
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
    startNode.h = Distances::GetManhattanDistanceTo(startX, startY, goalX, goalY);
    startNode.parentCoords = SDL_Point { startX, startY };
    startNode.openedStamp = nodePool.GetOpenStamp();

    openList.push(Node{ .coords = SDL_Point { startX, startY }, .f = startNode.g + startNode.h });

    const std::vector<SDL_Point> dirs = {
        SDL_Point {  0, -1 },
        SDL_Point {  0,  1 },
        SDL_Point { -1,  0 },
        SDL_Point {  1,  0 }
    };

    while (!openList.empty())
    {
        Node current = openList.top();
        openList.pop();

        GridNode& currentNode = nodePool.Get(current.coords);
        currentNode.closedStamp = nodePool.GetClosedStamp();

        // Reached goal.
        if (current.coords.x == goalX && current.coords.y == goalY)
        {
            // Reconstruct path.
            Path path;
            int cx = goalX;
            int cy = goalY;

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

        // Search neighbors.
        for (const SDL_Point& dir : dirs)
        {
            const int nx = current.coords.x + dir.x;
            const int ny = current.coords.y + dir.y;

            if (!map.IsInMapBounds(nx, ny) || map.IsTileSolid(nx, ny))
            {
                continue;
            }

            GridNode& neighbor = nodePool.Get(nx, ny);

            if (neighbor.closedStamp == nodePool.GetClosedStamp())
            {
                continue;
            }

            const int tentativeG = currentNode.g + 1;

            if (neighbor.openedStamp != nodePool.GetOpenStamp() || tentativeG < neighbor.g)
            {
                neighbor.g = tentativeG;
                neighbor.h = Distances::GetManhattanDistanceTo(nx, ny, goalX, goalY);
                neighbor.parentCoords = current.coords;

                if (neighbor.openedStamp != nodePool.GetOpenStamp())
                {
                    neighbor.openedStamp = nodePool.GetOpenStamp();
                    openList.push({ nx, ny, neighbor.g + neighbor.h });
                }
            }
        }
    }

    return {}; // No path found.
}