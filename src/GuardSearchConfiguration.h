#pragma once

#ifndef GUARDSEARCHCONFIGURATION_H_
#define GUARDSEARCHCONFIGURATION_H_

/**
* @brief Structure for guard search configuration.
*/
struct GuardSearchConfiguration
{
    int followTiles = 3;        /// The number of base tiles in the follow direction strategy.
    float randomSearchTime = 180.0f; /// The time spent searching in the random search strategy.
    float aggressionRandomTimeMultiplier = 180.0f;  /// The multiplier for the extra time spent randomly searching based on the guard's aggression.
};

#endif // GUARDSEARCHCONFIGURATION_H_