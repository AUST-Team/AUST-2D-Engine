#pragma once

#ifndef MAPSAVELOADSTRATEGY
#define MAPSAVELOADSTRATEGY

#include "MapLoadStrategy.h"
#include "GameSaveData.h"

/**
* @brief Strategy for loading maps from saves.
*/
class MapSaveLoadStrategy : public MapLoadStrategy
{
private:
	GameSaveData saveData;	/// Save data which to load from.

public:

	/**
	* @brief Constructor.
	* 
	* @param saveData The saved data which to load the map from.
	*/
	explicit MapSaveLoadStrategy(const GameSaveData& saveData);

	/**
	* @brief Default destructor.
	*/
	~MapSaveLoadStrategy() override = default;

	/**
	* @brief Loads the map from a save.
	* 
	* @param map Reference to the map to load.
	*/
	void LoadMap(Map& map) override;

	/**
	* @brief Sets the new saved data.
	* 
	* @param newData New saved data.
	*/
	void SetSaveData(const GameSaveData& newData);

	/**
	* @brief Returns the saved data.
	* 
	* @return The saved data struct.
	*/
	GameSaveData GetSaveData() const;
};

#endif // MAPSAVELOADSTRATEGY