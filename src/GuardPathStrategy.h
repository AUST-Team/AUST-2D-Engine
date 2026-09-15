#pragma once

#ifndef GUARDPATHSTRATEGY_H_
#define GUARDPATHSTRATEGY_H_

#include <SDL3/SDL_rect.h>

#include "GuardStrategy.h"
#include "Path.h"
#include "PathStrategy.h"

/**
* @brief Strategy for a guard to move towards a point (tile index). It automatically creates a path towards the tile.
* 
* Has pathfinding.
* 
* Non-shareable.
*/
class GuardPathStrategy : public GuardStrategy
{
protected:
    SDL_Point target;                   /// The tile index to move towads.
    Path path;                          /// Cached path (sequence of tiles).
    size_t currentIndex = 0;            /// Current node in path.
    PathStrategy pathfindingMethod;     /// A* or JPS.
    bool reachedPointFlag = false;      /// Flag if the guard has reached the DESTINATION(!) point.

public:

    /**
    * @brief Constructor.
    * 
    * @param destination Target point to move towards as in tile index.
    * @param strategy Strategy of pathfinding to use.
    */
    explicit GuardPathStrategy(const SDL_Point& destination = { 0, 0 } , PathStrategy strategy = PathStrategy::AStar);

    /**
    * @brief Constructor.
    *
    * @param targetX X coordinate of the target (tile index) to move towards.
    * @param targetY Y coordinate of the target (tile index) to move towards.
    * @param strategy Strategy of pathfinding to use.
    */
    explicit GuardPathStrategy(int targetX, int targetY, PathStrategy strategy = PathStrategy::AStar);

    /**
    * @brief Default destructor.
    */
    virtual ~GuardPathStrategy() override = default;

    /**
    * @brief Returns the direction the guard should take next.
    * 
    * @param guard Reference to the guard.
    * 
    * @return A direction for the guard to take, or Direction::ENUM_SENTINEL_VALUE if the guard should not move, or is stuck.
    */
    virtual Direction GetDirection(Guard& guard) override;

    /**
    * @brief Does nothing as this strategy doesn't rely on delta time.
    *
    * @param delteTime The delta time of the main game loop.
    */
    virtual void Update(double deltaTime) override;

    /**
    * @brief Sets a new target to move towards.
    *
    * Forces to remake the path.
    *
    * @param x X index of the tile to move towards.
    * @param y Y index of the tile to move towards.
    */
    virtual void SetTarget(int x, int y);

    /**
    * @brief Sets a new target to move towards.
    * 
    * Forces to remake the path.
    * 
    * @param newTarget New target index to move towards.
    */
    virtual void SetTarget(const SDL_Point& newTarget);

    /**
    * @brief Checks if the guard has reached the point.
    * 
    * @return 'true' if the guard has reached the point, 'false' if otherwise.
    */
    virtual bool HasReachedPoint() const;

    /**
    * @brief Resets the reachedPointFlag; sets it as 'false'.
    */
    virtual void ResetReachedPointFlag();

    /**
    * @brief Forces the strategy to remake the path.
    */
    virtual void ForcePathReconstruction();
};

#endif // GUARDPATHSTRATEGY_H_