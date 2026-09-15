#pragma once

#ifndef ASTAR_H_
#define ASTAR_H_

#include "GridNodePool.h"
#include "Path.h"

class GameMap;

/**
 * @brief Optimized 4-direction A* pathfinding on a grid.
 */
class AStar
{
private:
    GridNodePool nodePool;  /// Grid node pool.

public:

    /**
     * @brief Computes a path from start to goal.
     * 
     * @param map Reference to the game map.
     * @param startX Start tile X.
     * @param startY Start tile Y.
     * @param goalX Goal tile X.
     * @param goalY Goal tile Y.
     * 
     * @return Path to the point (empty if no path found).
     */
    Path FindPath(const GameMap& map, int startX, int startY, int goalX, int goalY);
};

#endif // ASTAR_H_