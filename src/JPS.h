#pragma once

#ifndef JPS_H_
#define JPS_H_

#include <vector>
#include <optional>
#include <queue>

#include "GridNodePool.h"
#include "Path.h"
#include "Node.h"

class GameMap;

/**
 * @brief Jump Point Search (4-direction, 'no' diagonals).
 * 
 * It uses Chebyshev distances, so technically does check diagonals, but only to find a forced turn.
 * 
 * May or may not work. Use AStar instead.
 */
class JPS
{
private:
    GridNodePool nodePool; /// Grid node pool. 

    /**
    * @brief Jumps to the next point.
    * 
    * @param map Reference to the game map.
    * @param x Current X coordinate (tile).
    * @param y Current Y coordinate (tile).
    * @param dx Direction to jump in the X coordinate.
    * @param dy Direction to jump in the Y coordinate.
    * @param goalX X coordinate of the goal (tile).
    * @param goalY Y coordinate of the goal (tile).
    * 
    * @return The coordinate of the jump, or std::nullopt if no jump can be made.
    */
    std::optional<SDL_Point> Jump(const GameMap& map, int x, int y, int dx, int dy, int goalX, int goalY);

    /**
     * @brief Identifies the successor of a node.
     *
     * @param map Reference to the game map.
     * @param startX Start tile X.
     * @param startY Start tile Y.
     * @param goalX Goal tile X.
     * @param goalY Goal tile Y.
     * @param openList List of the open nodes.
     */
    void IdentifySuccessors(const GameMap& map, int x, int y, int goalX, int goalY, std::priority_queue<Node, std::vector<Node>, std::greater<Node>>& openList);

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

#endif // JPS_H_

