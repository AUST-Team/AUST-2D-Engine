#pragma once

#ifndef COLLISIONCELL_H_
#define COLLISIONCELL_H_

/**
* @brief Structure for a collision cell on a map.
*/
struct CollisionCell
{
    bool solidFlag = false;         /// Flag if the cell is solid.
    bool blocksVisionFlag = false;  /// Flag if the cell blocks vision.
};

#endif // COLLISIONCELL_H_