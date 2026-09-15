#pragma once

#ifndef TILECONFIGURATION_H_
#define TILECONFIGURATION_H_

/**
* @brief Structure for tile configuration.
* 
* Same story as WindowConfiguration, although struct Size exists, here for any other extensions.
*/
struct TileConfiguration
{
    int width = 48;         /// Width (in pixels) of a tile.
    int height = 48;        /// Height (in pixels) of a tile.
    float maxScale = 4.0f;  /// Max scaling of the tiles.
    float minScale = 0.5f;  /// Min scaling of the tiles.
    float step = 0.25f;     /// Step to take when calculating scales.
};

#endif // TILECONFIGURATION_H_