#pragma once

#ifndef TILEMETADATAPART_H_
#define TILEMETADATAPART_H_

#include <SDL3/SDL_rect.h>

/**
* @brief Structure containing metadata for a tile.
*/
struct TileMetadataPart
{
	SDL_Point offset = { 0, 0 };	/// Offset of the tile part (used for multi-part tiles).
	TileSoundType defaultSound = TileSoundType::None;		/// The default sounds, if any.
	TileSoundType allowedSoundsMask = TileSoundType::None;  /// Mask of allowed sounds.
};

#endif // TILEMETADATAPART_H_