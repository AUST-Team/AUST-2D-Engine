#pragma once

#ifndef GUARDTIMINGCONFIGURATION_H_
#define GUARDTIMINGCONFIGURATION_H_

/**
* @brief Structure for guard timings.
*/
struct GuardTimingConfiguration
{
    float alertDuration = 1500.0f;        /// Alert duration (in milliseconds).
    float aggressionReduction = 5000.0f;  /// Duration of aggression decrease (in milliseconds).
    float visionReduction = 10000.0f;     /// Duration of vision reduction (in milliseconds).
    float patrolPause = 1500.0f;          /// Duration of the pause when reaching a patrol point (in milliseconds).
};

#endif // GUARDTIMINGCONFIGURATION_H_