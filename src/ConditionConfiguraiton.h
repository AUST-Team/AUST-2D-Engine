#pragma once

#ifndef CONDITIONCONFIGURATION_H_
#define CONDITIONCONFIGURATION_H_

/**
* @brief Structure for conditions configuration.
*/
struct ConditionConfiguration
{
    int interactRadius = 1;    /// Default radius for the interact condition (1 tile).
    int facingRadius = 6;      /// Default radius for the facing conditon (6 tiles).
};

#endif // CONDITIONCONFIGURATION_H_