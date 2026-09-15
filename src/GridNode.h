#pragma once

#ifndef GRIDNODE_H_
#define GRIDNODE_H_

#include <SDL3/SDL_rect.h>
#include <limits>

/**
* @brief Structure for a grid node.
*/
struct GridNode
{
    int g = std::numeric_limits<int>::max(); /// Cost from the start.
    int h = -1;  /// Heuristic

    SDL_Point parentCoords = { -1, -1 }; /// Coordinates of the parent grid node.

    int openedStamp = -1;   /// Stamp of the search that opened the node.
    int closedStamp = -1;   /// Stamp of the search that closed the node.
};


#endif // GRIDNODE_H_