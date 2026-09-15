#include "TileAnchorInstance.h"

#include "SDL_PointOperators.h"

TileAnchorInstance::TileAnchorInstance(int anchorX, int anchorY, int tileIndex) :
	TileAnchorInstance(SDL_Point{ anchorX, anchorY }, tileIndex) {}

TileAnchorInstance::TileAnchorInstance(const SDL_Point& anchorPosition, int tileIndex) :
	anchorPosition(anchorPosition),
	tileIndex(tileIndex) {}

bool TileAnchorInstance::operator==(const TileAnchorInstance& b)
{
	return tileIndex == b.tileIndex && anchorPosition == b.anchorPosition;
}
