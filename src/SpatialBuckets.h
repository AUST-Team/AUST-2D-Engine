#pragma once

#ifndef SPATIALBUCKETS_H_
#define SPATIALBUCKETS_H_

#include "EnumMiscs.h"

struct SDL_Point;

/**
* @brief Spatial buckets to categorize conditions (and implicitely triggers), to make updates faster.
* 
* A trigger will only try to fire if it close enough to the player based on the spatial bucket.
* 
* These should be order of [lowest range -> highest range]
*/
enum class SpatialBucket : uint8_t
{
    TileOnly,   /// Bucket for tile only (stepped on).
    Near,       /// Bucket for near tiles.
    Medium,     /// Bucket for medium tiles.
    Far,        /// Bucket for far tiles.
    COUNT       /// Number of elements.
};

template <>
inline constexpr bool isCountableEnum<SpatialBucket> = true;

/**
* @brief Returns the corresponding spatial bucket for a radius.
* 
* @return The corresponding spatial bucket for a radius.
*/
SpatialBucket GetSpatialBucketFromRadius(uint64_t radius);

/**
* @brief Returns the radius of a spatial bucket.
* 
* @return The radius of a spatial bucket.
*/
uint64_t GetRadiusFromSpatialBucket(SpatialBucket bucket);

/**
* @brief Checks if a point is within bucket range. Uses Manhattan distance.
*
* @param bucketX X coordinate of the bucket's center.
* @param bucketY Y coordinate of the bucket's center.
* @param pointX X coordinate of the point.
* @param pointY Y coordinate of the point.
* @param bucketType The type of bucket.
*
* @return 'true' if the point is within bucket range, 'false' if otherwise.
*/
bool IsWithinBucketRange(int bucketX, int bucketY, int pointX, int pointY, SpatialBucket bucketType);

/**
* @brief Checks if a point is within bucket range. Uses Manhattan distance.
* 
* @param bucketPosition The position of the bucket's center.
* @param pointPosition The position of the bucket to check.
* @param bucketType The type of bucket.
* 
* @return 'true' if the point is within bucket range, 'false' if otherwise.
*/
bool IsWithinBucketRange(const SDL_Point& bucketPosition, const SDL_Point& pointPosition, SpatialBucket bucketType);

#endif // SPATIALBUCKETS_H_