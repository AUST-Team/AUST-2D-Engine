#pragma once

#ifndef GUARDPATHCHASESTRATEGY_H_
#define GUARDPATHCHASESTRATEGY_H_

#include "GuardPathStrategy.h"

/**
* @brief Strategy for a chase that uses pathfinding.
* 
* Has pathfinding.
* 
* Semi-shareable (depending if you want the threshold to be global or per-guard).
*/
class GuardPathChaseStrategy : public GuardPathStrategy
{
private:
    int recalcDistanceThreshold; /// The number of tiles the player should be away from the last point of the path in order to call a recalculation.

public:

    /**
    * @brief Constructor.
    * 
    * @param strategy Pathfinding strategy to be used.
    * @param treshholdTiles How many tiles should the player be way from the last point to trigger a reconstruction of the path.
    */
    GuardPathChaseStrategy(PathStrategy strategy = PathStrategy::AStar, int thresholdTiles = 2);

    /**
    * @brief Returns the direction the guard should take to reach the player.
    * 
    * @param guard Reference to the guard.
    * 
    * @return A direction for the guard to take, or Direction::ENUM_SENTINEL_VALUE if the guard should not move, or is stuck.
    */
    Direction GetDirection(Guard& guard) override;
};

#endif GUARDPATHCHASESTRATEGY_H_
