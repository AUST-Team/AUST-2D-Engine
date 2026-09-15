#pragma once

#ifndef VISIONRANGEMODIFIERCONFIGURATION_H_
#define VISIONRANGEMODIFIERCONFIGURATION_H_

/**
* @brief Modifier for a vision range.
*/
struct VisionRangeModifierConfiguration 
{
    float additionalAngle = 0.0f;   /// Additional angle (applied to all vision cones).
    int additionalTiles = 0;        /// Additional tiles (applied to all vision ranges).
};

#endif // VISIONRANGEMODIFIERCONFIGURATION_H_