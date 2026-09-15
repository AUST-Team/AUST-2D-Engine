#include "GridNodePool.h"

void GridNodePool::ResizeIfNeeded(int mapWidth, int mapHeight)
{
    if (mapWidth <= width && mapHeight <= height)
    {
        // Already large enough.
        return; 
    }

    width = mapWidth;
    height = mapHeight;
    nodes.resize(static_cast<size_t>(width) * height);
}

void GridNodePool::BeginSearch()
{
    // Increment stamps to avoid clearing the grid.
    openStamp += 2;
    closedStamp += 2;

    /*
        Stamps are akin to identifiers for the nodes. They help with reusing the nodes in order to not be required to allocate / deallocate
            nodes
        If node.openStamp == pool.GetOpenStamp()        <=> this node was opened by the current search (valid node to be visited)
        if node.closedStamp == pool.GetClosedStamp()    <=> this node was closed by the current search (visited, don't go over again)
        if node.openStamp != pool.GetOpenStamo()        <=> this node was NOT open by the current search (can be reused or added to open set)
        if node.closedStamp != pool.GetClosedStamp()    <=> this node was NOT closed by the current search (can be visited)
    */

    // Prevent overflow (rare).
    if (openStamp < 0 || closedStamp < 0)
    {
        openStamp = 1;
        closedStamp = 2;

        for (GridNode& n : nodes)
        {
            n.openedStamp = -1;
            n.closedStamp = -1;
        }
    }
}

GridNode& GridNodePool::Get(int x, int y) { return nodes[Index(x, y)]; }

GridNode& GridNodePool::Get(const SDL_Point& pos) { return Get(pos.x, pos.y); }

int GridNodePool::Index(int x, int y) const { return y * width + x; }

int GridNodePool::GetOpenStamp() const { return openStamp; }

int GridNodePool::GetClosedStamp() const { return closedStamp; }

int GridNodePool::GetWidth() const { return width; }

int GridNodePool::GetHeight() const { return height; }

