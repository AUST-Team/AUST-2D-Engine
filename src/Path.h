#pragma once

#ifndef PATH_H_
#define PATH_H_

#include <SDL3/SDL_rect.h>
#include <vector>

/**
* @brief Structure for a path.
*/
struct Path
{
    std::vector<SDL_Point> nodes; /// Vector of tiles (nodes).
    
    /**
    * @brief Checks if the path is empty.
    * 
    * @return 'true' if the path is empty, 'false' if otherwise.
    */
    constexpr inline bool Empty() const
    {
        return nodes.empty();
    }

    /**
    * @brief Clears the path nodes.
    */
    constexpr inline void Clear()
    {
        nodes.clear();
    }
};

#endif // PATH_H_

