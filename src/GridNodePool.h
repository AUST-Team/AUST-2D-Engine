#pragma once

#ifndef GRIDNODEPOOL_H_
#define GRIDNODEPOOL_H_

#include <vector>

#include "GridNode.h"

/**
 * @brief Manages a reusable grid of nodes for A* / JPS pathfinding.
 *
 * This avoids allocations every frame and uses a stamp system to avoid clearing
 * thousands of nodes per search.
 */
class GridNodePool
{
private:
    std::vector<GridNode> nodes;    /// Vector ('grid') of nodes.

    int width = 0;  /// Width of the grid.
    int height = 0; /// Height of the grid.

    int openStamp = 1;      /// Stamp for what search has opened the node.
    int closedStamp = 2;    /// Stamp for what search has closed the node.

public:

    /**
     * @brief Ensures the pool matches the map size.
     *
     * @param mapWidth  Width of the map in tiles.
     * @param mapHeight Height of the map in tiles.
     */
    void ResizeIfNeeded(int mapWidth, int mapHeight);

    /**
     * @brief Begin a new pathfinding search. Increments stamps.
     */
    void BeginSearch();

    /**
     * @brief Returns a node at (x, y).
     * 
     * \/!\ No bounds checking for performance — caller must ensure valid coords. \/!\
     * 
     * @param x X coordinate of the node.
     * @param y Y coordinate of the node.
     * 
     * @return The grid node at (x, y);
     */
    GridNode& Get(int x, int y);

    /**
     * @brief Returns a node at (x, y).
     *
     * \/!\ No bounds checking for performance — caller must ensure valid coords. \/!\
     *
     * @param pos Coordinates of the node.
     * 
     * @return The grid node at (x, y);
     */
    GridNode& Get(const SDL_Point& pos);

    /**
     * @brief Converts (x, y) into flat array index.
     * 
     * @param x Column of the node.
     * @param y Row of the node.
     * 
     * @return The index of the flat array containing the node at (x, y).
     */
    int Index(int x, int y) const;

    /**
    * @brief Returns the number of open stamps.
    * 
    * @return The number of open stamps.
    */
    int GetOpenStamp() const;

    /**
    * @brief Returns the nunber of closed stamps.
    * 
    * @return The number of closed stamps.
    */
    int GetClosedStamp() const;

    /**
    * @brief Returns the width of the grid.
    * 
    * @return The width of the grid.
    */
    int GetWidth() const;

    /**
    * @brief Returns the height of the grid.
    * 
    * @return The height of the grid.
    */
    int GetHeight() const;
};

#endif // GRIDNODEPOOL_H_
