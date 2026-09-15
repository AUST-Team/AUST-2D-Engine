#include <SDL3/SDL.h>
#include <cmath>

#include "SpatialBuckets.h"

#include "Configuration.h"

SpatialBucket GetSpatialBucketFromRadius(uint64_t radius)
{
	const GameConfiguration& config = Configuration::Get();
	uint64_t tileRadius = config.spatialBuckets.tileOnlyRadius;
	uint64_t nearRadius = config.spatialBuckets.nearRadius;
	uint64_t mediumRadius = config.spatialBuckets.mediumRadius;

	if (radius == tileRadius)
	{
		return SpatialBucket::TileOnly;
	}
	else if (radius <= nearRadius)
	{
		return SpatialBucket::Near;
	}
	else if (radius <= mediumRadius)
	{
		return SpatialBucket::Medium;
	}
	else
	{
		return SpatialBucket::Far;
	}
}

uint64_t GetRadiusFromSpatialBucket(SpatialBucket bucket)
{
	const GameConfiguration& config = Configuration::Get();
	uint64_t tileRadius = config.spatialBuckets.tileOnlyRadius;
	uint64_t nearRadius = config.spatialBuckets.nearRadius;
	uint64_t mediumRadius = config.spatialBuckets.mediumRadius;
	uint64_t farRadius = config.spatialBuckets.farRadius;

	switch (bucket)
	{
		case SpatialBucket::TileOnly:	return tileRadius;
		case SpatialBucket::Near:		return nearRadius;
		case SpatialBucket::Medium:		return mediumRadius;
		case SpatialBucket::Far:		return farRadius;
		default:
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GetRadiusFromSpatialBucket: Unknown type of bucket. Defaulting to TileOnly.");
			return tileRadius;
		}
	}
}

bool IsWithinBucketRange(int bucketX, int bucketY, int pointX, int pointY, SpatialBucket bucketType)
{
	uint64_t maxRange = GetRadiusFromSpatialBucket(bucketType);

	const int dx = std::abs(bucketX - pointX);
	const int dy = std::abs(bucketY - pointY);

	return (static_cast<int64_t>(dx) + dy) <= static_cast<int64_t>(maxRange);
}

bool IsWithinBucketRange(const SDL_Point& bucketPosition, const SDL_Point& pointPosition, SpatialBucket bucketType)
{
	return IsWithinBucketRange(bucketPosition.x, bucketPosition.y, pointPosition.x, pointPosition.y, bucketType);
}
