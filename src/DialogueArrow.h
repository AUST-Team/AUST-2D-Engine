#pragma once

#ifndef DIALOGUEARROW_H_
#define DIALOGUEARROW_H_

#include "ConstantConfiguration.h"

/**
* @brief Stucture for a dialogue arrow.
*/
struct DialogueArrow
{
	int tileIndex = ConstantConfiguration::invalidTileIndex;	/// Tile index of the arrow.
	uint8_t transparency = 255;	/// Transparency of the arrow.
};

#endif // DIALOGUEARROW_H_