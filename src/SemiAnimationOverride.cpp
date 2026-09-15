#include "SemiAnimationOverride.h"

SemiAnimationOverride::SemiAnimationOverride(int tileIndex, TileAnimationType type, float startTime, bool oneShot) :
	tileIndex(tileIndex),
	startTime(startTime),
	oneShotFlag(oneShot),
	type(type) {}
