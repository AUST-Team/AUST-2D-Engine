#include <SDL3/SDL_log.h>

#include "FullAnimationOverride.h"

#include "TileRegistry.h"

FullAnimationOverride::FullAnimationOverride(int tileIndex, TileAnimationType type, bool oneShot) :
	tileIndex(tileIndex),
	type(type),
	oneShotFlag(oneShot) 
{
	TileInstance* tile = TileRegistry::GetTileTypeByIndex(tileIndex);
	if (!tile)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "FullAnimationOverride.FullAnimationOverride: Invalid tile index.");
		return;
	}
	else
	{
		if (!HasFlag(tile->animationMask, type))
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "FullAnimationOverride.FullAnimationOverride: Tile doesn't have specified flag.");
			return;
		}
		clip = tile->animationTable[TileReactionIndexFromType(type)];
	}
}
