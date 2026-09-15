#pragma once

#ifndef GUARDCONFIGURATION_H_
#define GUARDCONFIGURATION_H_

#include "VisionRange.h"
#include "VisionRangeConfiguration.h"
#include "GuardTimingConfiguration.h"

/**
* @brief Configuration for a guard.
*/
struct GuardConfiguration 
{
    float patrolSpeed = 2.0f;   /// Patrol speed (in pixels).
    float chaseSpeed = 2.4f;    /// Chase speed (in pixels).

    VisionRangeConfiguration vision;    /// Vision range configuration.

    GuardTimingConfiguration timings;   /// Timings for the guard.

    /**
    * @brief Returns the modified vision range with a certain modification.
    * 
    * @param mod Vision range modifier. Will not be modified itself.
    * 
    * @return The modified vision range (base + mod).
    */
    VisionRange MakeVisionRange(const VisionRangeModifierConfiguration& mod) const;
};

#endif GUARDCONFIGURATION_H_