#pragma once

#ifndef NODE_H_
#define NODE_H_

#include <SDL3/SDL_rect.h>

/**
* @brief Structure that defines a node used in AStar and JPS algorithms.
*/
struct Node
{
    SDL_Point coords = { -1, -1 }; /// Coordinates of the node (tile).
    int f = -1;  /// Total cost of the node (f = g + h).

    /**
    * @brief 'Greater than' operator.
    *
    * @return 'true' if this->f > other.f, 'false' otherwise.
    */
    constexpr inline bool operator>(const Node& other) const
    {
        return f > other.f;
    }
};

#endif // NODE_H_
