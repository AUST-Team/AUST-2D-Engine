#pragma once

#ifndef PATHSTRATEGY_H_
#define PATHSTRATEGY_H_

/**
* @brief Enum of possible path strategies.
*/
enum class PathStrategy : uint8_t
{
    AStar,  /// AStar algorithm.
    JPS,    /// JPS algorithm.
    COUNT   // Number of elements.
};

#endif // PATHSTRATEGY_H_