#pragma once

#ifndef MAPLOADSTRATEGY_H_
#define MAPLOADSTRATEGY_H_

class Map;

/**
* @brief Abstract class for a map initialisation strategy.
*/
class MapLoadStrategy
{
public:

	/**
	* @brief Default destructor.
	*/
	virtual ~MapLoadStrategy() = default;

	/**
	* @brief Loads the map.
	* 
	* @param map Reference to the map to load.
	*/
	virtual void LoadMap(Map& map) = 0;
};

#endif // MAPLOADSTRATEGY_H_