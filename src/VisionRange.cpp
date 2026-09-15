#include "VisionRange.h"

VisionRange& VisionRange::operator+=(int tiles)
{
	tileRanges.centralTileRange += tiles;
	tileRanges.normalTileRange += tiles;
	tileRanges.peripheralTileRange += tiles;

	return *this;
}

VisionRange& VisionRange::operator+=(float angle)
{
	coneAngles.centralConeAngle += angle;
	coneAngles.normalConeAngle += angle;
	coneAngles.peripheralConeAngle += angle;

	return *this;
}
