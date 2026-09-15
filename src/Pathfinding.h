#pragma once

#ifndef PATHFINDING_H_
#define PATHFINDING_H_

#include "AStar.h"
#include "JPS.h"
#include "GameMap.h"
#include "Direction.h"
#include "PathStrategy.h"

class Guard;
class Player;

/**
* @brief Singleton that holds the pathfinding algorithms.
*/
class Pathfinding
{
private:
    AStar astar;    /// AStar algorithm.
    JPS jps;        /// JPS algorithm (not recommended).

    /**
    * @brief Default constructor.
    */
    Pathfinding() = default;

    /**
    * @brief Default destrcutor.
    */
    ~Pathfinding() = default;

    // No copying or moving allowed due to singleton pattern.
    Pathfinding(const Pathfinding&) = delete;
    Pathfinding& operator=(const Pathfinding&) = delete;
    Pathfinding(Pathfinding&&) = delete;
    Pathfinding& operator=(Pathfinding&&) = delete;

public:

    /**
    * @brief Returns the created pathfinder instance.
    *
    * @return Reference to the pathfinder instance.
    */
    static Pathfinding& GetInstance() noexcept
    {
        static Pathfinding instance;
        return instance;
    }

    /**
     * @brief Finds a path using the chosen strategy.
     *
     * @param startX Start tile X.
     * @param startY Start tile Y.
     * @param goalX Goal tile X.
     * @param goalY Goal tile Y.
     * @param strategy Pathfinding strategy to use.
     *
     * @return The path to the goal tile (empty if no path found).
     */
    Path FindPath(int startX, int startY, int goalX, int goalY, PathStrategy strategy = PathStrategy::AStar);

    /**
     * @brief Finds a path using the chosen strategy.
     * 
     * @param map Reference to the map.
     * @param startX Start tile X.
     * @param startY Start tile Y.
     * @param goalX Goal tile X.
     * @param goalY Goal tile Y.
     * @param strategy Pathfinding strategy to use.
     * 
     * @return The path to the goal tile (empty if no path found).
     */
    Path FindPath(const GameMap& map, int startX, int startY, int goalX, int goalY, PathStrategy strategy = PathStrategy::AStar);

    /**
    * @brief Finds the next DIRECTION(!) for the guard.
    * 
    * @param guard Reference to the guard.
    * @param player Pointer to the player.
    * @param strategy Path-finding strategy to be used.
    * 
	* @return The next direction for the guard to take, or Direction::ENUM_SENTINEL_VALUE if no path can be found.
    */
    Direction FindNextDirectionForGuard(Guard& guard, const Player* player, PathStrategy strategy = PathStrategy::AStar);
};

#endif // PATHFINDING_H_
