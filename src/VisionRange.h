#pragma once

#ifndef VISIONRANGE_H_
#define VISIONRANGE_H_

/**
* @brief Structure that holds a vision range as a cone.
*/
struct VisionRange
{
	/**
	* @brief Structure containing the cone angles of the vision range.
	*/
	struct ConeAngles
	{
		float centralConeAngle;		/// The central cone angle, where enemy vision is the longest.
		float normalConeAngle;		/// The normal cone angle, where enemy vision is standard.
		float peripheralConeAngle;	/// The peripheral cone angle, where enemy vision is the shortest.
	} coneAngles;
	
	/**
	* @brief Structure containing the tile ranges of the vision range.
	*/
	struct TileRanges
	{
		int centralTileRange;		/// The number of tiles an enemy can see in the central cone.
		int normalTileRange;		/// The number of tiles an enemy can see in the normal cone.
		int peripheralTileRange;	/// The number of tiles an enemy can see in the peripheral cone.
	} tileRanges;

	/**
	* @brief Constructor.
	* 
	* @param centralConeAngle The angle of the central vision cone.
	* @param normalConeAngle The angle of the normal vision cone.
	* @param peripheralConeAngle The angle of the peripheral vision cone.
	* @param centralTileRange The tile range of the central vision cone.
	* @param normalTileRange The tile range of the normal vision cone.
	* @param peripheralTileRange The tile range of the peripheral vision cone.
	*/
	constexpr VisionRange(float centralConeAngle, float normalConeAngle, float peripheralConeAngle, int centralTileRange, int normalTileRange, int peripheralTileRange) :
		coneAngles({ centralConeAngle, normalConeAngle, peripheralConeAngle }),
		tileRanges({ centralTileRange, normalTileRange, peripheralTileRange }) {}

	/**
	* @brief Plus equals operator. Adds a number of tiles to the tile ranges.
	* 
	* @param tiles The number of tiles to add.
	*/
	VisionRange& operator+=(int tiles);

	/**
	* @brief Plus equals operator. Adds an angle to the cone angles.
	*
	* @param angle The angle in degrees to add.
	*/
	VisionRange& operator+=(float angle);
};

#endif // VISIONRANGE_H_
