#pragma once

#ifndef VISIONRANGEBASECONFIGURATION_H_
#define VISIONRANGEBASECONFIGURATION_H_

/**
* @brief Structure for a vision range's base components.
*/
struct VisionRangeBaseConfiguration
{
    float centralAngle = 30.0f;     /// Central vision cone (in degrees).
    float coneAngle = 90.0f;        /// Normal vision cone (in degrees).
    float peripheralAngle = 140.0f; /// Peripheral vision cone (in degrees).
    int centralRange = 4;           /// Central vision range (in tiles).
    int coneRange = 3;              /// Normal vision range (in tiles).
    int peripheralRange = 2;        /// Peripheral vision range (in tiles).
};

#endif // VISIONRANGEBASECONFIGURATION_H_