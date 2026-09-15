#pragma once

#ifndef SPATIALBUCKETCONFIGURATION_H_
#define SPATIALBUCKETCONFIGURATION_H_

/**
* @brief Configuration for space buckets. A point is in the lowest bucket possible.
*/
struct SpatialBucketConfiguration
{
    uint64_t tileOnlyRadius = 0;    /// Default radius for a 'TileOnly' spatial bucket (in tiles).
    uint64_t nearRadius = 2;        /// Default radius for a 'Near' spatial bucket (in tiles).
    uint64_t mediumRadius = 5;      /// Default radius for a 'Medium' spatial bucket (in tiles).
    uint64_t farRadius = 6;         /// Default radius for a 'Far' spatial bucket (in tiles; 6+ tiles).
};

#endif // SPATIALBUCKETCONFIGURATION_H_