#pragma once

#ifndef VISIONRANGECONFIGURATION_H_
#define VISIONRANGECONFIGURATION_H_

#include "VisionRangeBaseConfiguration.h"
#include "VisionRangeModifierConfiguration.h"

/**
* @brief Vision range configuration.
*/
struct VisionRangeConfiguration
{
    VisionRangeBaseConfiguration base;    /// Base vision of the guard.
    VisionRangeModifierConfiguration patrol;  /// Modifier for the vision range when patrolling (it's technically the base, but here for consistency).
    VisionRangeModifierConfiguration chase;   /// Modifier for the vision range when chasing.
    VisionRangeModifierConfiguration search;  /// Modifier for the vision range when searching.
};

#endif //VISIONRANGECONFIGURATION_H_