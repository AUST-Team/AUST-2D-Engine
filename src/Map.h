#pragma once

#ifndef MAP_H_
#define MAP_H_

#include <string>
#include <vector>
#include <SDL3/SDL_rect.h>
#include <nlohmann/json.hpp>
#include <filesystem>

#include "Size.h"
#include "Guard.h"
#include "SDL_PointOperators.h"
#include "ConditionRegistry.h"
#include "SpatialBuckets.h"
#include "TriggerRegistry.h"
#include "TilePlacementMode.h"
#include "DialogueData.h"
#include "TileSaveData.h"
#include "PresetSaveData.h"

class Trigger;

/**
* @brief Describes a map.
* 
* Map owns triggers. ConditionRegistry owns base conditions (non composite). Triggers (through ActivationExpression) has a condition that might or might not be owned.
*/
class Map
{
private:
    TriggerRegistry triggerRegistry;      /// Trigger registry for the map. DOESN'T OWN THE TRIGGERS!
    ConditionRegistry conditionRegistry;  /// Condition registry for the map.
    std::unordered_map<SDL_Point, std::vector<Trigger *>> triggersByPosition;   /// Map of triggers using position; [SDL_Point; vector<Trigger *>]. OWNS THEM!
    
    /**
    * @brief Structure for more readibility.
    */
    struct MapCell
    {
        std::vector<int> tileIndexes; /// Tile indexes.
    };
    
    std::vector<MapCell> tiles;  /// The tiles on the map.

    std::string name;   /// Name of the map.
    Size size = { 0, 0 };   /// Size of the map.
    SDL_Point playerStart = { 0, 0 };   /// Player spawn point (tile index).
    Direction playerFacingDirection = GetSentinel<Direction>();  /// The player facing position.

	DialogueData defaultDialogue;   /// The default dialogue for the map.
    
    /**
    * @brief Loads map details from JSON.
    * 
    * @param mapDetailsJSON The JSON file containing the map details.
    */
    void LoadMapDetails(const nlohmann::json& mapDetailsJSON);

    /**
    * @brief Loads map tiles from JSON.
    *
    * @param mapTilesJSON The JSON file containing the map tiles.
    */
    void LoadMapTiles(const nlohmann::json& mapTilesJSON);

    /**
    * @brief Loads map guards from JSON.
    *
    * @param mapGuardsJSON The JSON file containing the map guards.
    */
    void LoadMapGuards(const nlohmann::json& mapGuardsJSON);

    /**
    * @brief Loads map triggers from JSON.
    * 
    * @param mapTriggersJSON The JSON file containing the map triggers.
    */
    void LoadMapTriggers(const nlohmann::json& mapTriggersJSON);

    /**
    * @brief Loads map presets from JSON.
    * 
    * @param mapPresentsJSON The JSON file containing the map presets.
    */
    void LoadMapPresets(const nlohmann::json& mapPresentsJSON);

    /**
    * @brief Loads player details from JSON.
    * 
    * @param playerDetailsJSON The JSON file containing the player details.
    */
    void LoadPlayerDetails(const nlohmann::json& playerDetailsJSON);

    /**
    * @brief Cleans up triggers that should auto-delete.
    */
    void CleanupAutoDeletedTriggers();

    /**
    * @brief Resolves JSON coordinate for tiles.
    * 
    * @param posJSON JSON containing the coordinates of tiles.
    * @param key Key ("x", "y", etc) of the coordinate.
    * @param max Maximum size allowed (ie map sizes).
    * 
    * @return A vector containing the positions of the tiles.
    */
    std::vector<int> ResolvePositions(const nlohmann::json& posJSON, const std::string& key, int max);

    /**
    * @brief Returns the index to be used in the map.
    * 
    * @param x Column of the index.
    * @param y Row of the index.
    * 
    * @return The corresponding index (y * width + x).
    */
    size_t Index(int x, int y) const;

    // No copying due to ConditionRegistry being unable to be copied.
    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;

public:

    /**
    * @brief Default construtor.
    */
    Map() = default;

    /**
    * @brief Move constructor.
    */
    Map(Map&& other) noexcept;

    /**
    * @brief Default destructor
    */
    ~Map();

    /**
    * @brief Updates the map.
    * 
    * @param deltaTime The delta time of the main game loop.
    * @param player Pointer to the player.
    * @param pendingUpdates Bitmask of the pending updates.
    */
    void Update(double deltaTime, Player* player, MapUpdateFlags pendingUpdates = MapUpdateFlags::None);

    /**
    * @brief Places the player at the start of the map.
    */
    void PlacePlayerAtStart() const;

    /**
    * @brief Places the player at a designated point.
    * 
    * @param x X coordinate of the point.
    * @param y Y coordinate of the point.
    */
    void PlacePlayerAt(float x, float y) const;

    /**
    * @brief Places the player at a designated point.
    * 
    * @param point Point to place the player at.
    */
    void PlacePlayerAt(const SDL_FPoint& point) const;

    /**
    * @brief Places the player at a designated tile.
    *
    * @param x X coordinate (tile index) of the point.
    * @param y Y coordinate (tie index) of the point.
    */
    void PlacePlayerAtTile(int x, int y) const;

    /**
    * @brief Places the player at a designated tile.
    *
    * @param point Point (tile index) to place the player at.
    */
    void PlacePlayerAtTile(const SDL_Point& point) const;

    /**
    * @brief Returns the map's name.
    * 
    * @return The map name.
    */
    std::string GetName() const;

    /**
    * @brief Returns the first tile at the specified coordinates.
    * 
    * @param x X coordinate of the tile.
    * @param y Y coordinate of the tile.
    * 
    * @return The ID of the tile at the specified ID, or the default invalid tile ID if it's out-of-bounds.
    */
    int GetTile(int x, int y) const;

    /**
    * @brief Returns the first tile at the specified coordinates.
    *
    * @param tilePosition The position of the tile.
    *
    * @return The ID of the tile at the specified ID, or the default invalid tile ID if it's out-of-bounds.
    */
    int GetTile(const SDL_Point& tilePosition) const;

    /**
    * @brief Returns the tiles at the specified coordinates.
    *
    * @param x X coordinate of the tiles.
    * @param y Y coordinate of the tiles.
    *
    * @return Reference to the vector containing the tiles at the specified coordinates.
    */
    const std::vector<int>& GetTiles(int x, int y) const;

    /**
    * @brief Returns the tiles at the specified coordinates.
    *
    * @param tilePosition The position of the tiles.
    *
    * @return Reference to the vector containing the tiles at the specified coordinates.
    */
    const std::vector<int>& GetTiles(const SDL_Point& tilePosition) const;

    /**
    * @brief Sets a tile at the specified coordinates.
    * 
    * @param x X coordinate of the tile.
    * @param y Y coordinate of the tile.
    * @param tileIndex Index of the tile.
    * @param placementMode Placement mode of the tile.
    */
    void SetTile(int x, int y, int tileIndex, TilePlacementMode placementMode);

    /**
    * @brief Sets a tile at the specified coordinates.
    * 
    * @param tilePosition The position of the tile.
    * @param tileIndex Index of the tile.
    * @param placementMode Placement mode of the tile.
    */
    void SetTile(const SDL_Point& tilePosition, int tileIndex, TilePlacementMode placementMode);

    /**
    * @brief Sets a tile at the specified coordinates.
    * 
    * @param x X coordinate of the tile.
    * @param y Y coordinate of the tile.
    * @param tileName Name of the tile.
    * @param placementMode Placement mode of the tile.
    */
    void SetTile(int x, int y, const std::string& tileName, TilePlacementMode placementMode);

    /**
    * @brief Sets a tile at the specified coordinates.
    *
    * @param tilePosition The position of the tile.
    * @param tileName Name of the tile.
    * @param placementMode Placement mode of the tile.
    */
    void SetTile(const SDL_Point& tilePosition, const std::string& tileName, TilePlacementMode placementMode);

    /**
	* @brief Sets a tile at the specified coordinates using JSON.
    * 
	* @param tileJSON JSON object containing the tile information.
    * @param [out] outChanges Optional vector of tile changes. If set to nullptr, it will not record anything.
    * Vector will NOT be cleared. Any changes will be added ontop of existing elements.
    */
    void SetTile(const nlohmann::json& tileJSON, std::vector<TileSaveData>* outChanges = nullptr);

    /**
    * @brief Returns the width of the map in tiles.
    * 
    * @return The width of the map in tiles.
    */
    int GetWidth() const;

    /**
    * @brief Returns the height of the map in tiles.
    *
    * @return The height of the map in tiles.
    */
    int GetHeight() const;

    /**
    * @brief Resizes the map.
    * 
    * @param newSize New size of the map.
    */
    void Resize(Size newSize);

    /**
    * @brief Adds a trigger to the map
    *
    * @param x X coordinate of the trigger.
    * @param y Y coordinate of the trigger.
    * @param trigger Pointer to the Trigger to place.
    */
    void SetTrigger(int x, int y, Trigger* trigger);

    /**
    * @brief Adds a trigger to the map
    *
    * @param tilePosition The position of the trigger.
    * @param trigger Pointer to the Trigger to place.
    */
    void SetTrigger(const SDL_Point& tilePosition, Trigger* trigger);

    /**
    * @brief Adds a trigger to the map.
    * 
    * @param triggerJSON JSON containing the trigger data.
    * 
    * @return The created trigger, or nullptr if trigger cannot be created.
    */
    Trigger* SetTrigger(const nlohmann::json& triggerJSON);

    /**
    * @brief Return a trigger from the map
    *
    * @param x X coordinate of the trigger.
    * @param y Y coordinate of the trigger.
    * 
    * @return A vector of the trigger pointers on the tile, or an empty vector if none exist.
    */
    std::vector<Trigger *>& GetTriggers(int x, int y);

    /**
    * @brief Return a trigger from the map
    *
    * @param tilePosition The position of the trigger.
    *
    * @return A vector of the trigger pointers on the tile, or an empty vector if none exist.
    */
    std::vector<Trigger *>& GetTriggers(const SDL_Point& tilePosition);

    /**
    * @brief Returns all triggers from the map.
    *
    * @return All triggers from the map.
    */
    std::vector<Trigger *> GetAllTriggers();

    /**
    * @brief Returns a trigger by ID.
    * 
    * @param triggerId ID of the trigger.
    * 
    * @return Pointer to the trigger, or nullptr if it doesn't exist.
    */
    Trigger* GetTriggerById(uint64_t triggerId);

    /**
    * @brief Removes a trigger from the map.
    * 
    * @param trigger Pointer to the trigger to remove.
    */
    void RemoveTrigger(Trigger* trigger);

    /**
    * @brief Applies a preset.
    *
    * @param posX X coordinate of the top left corner of the preset.
    * @param posY Y coordinate of the top left corner of the preset.
    * @param presetName Name of the preset.
    * @param placementMode Placement mode of the tiles. Set ::UseTileDefault to use tile default.
    */
    void SetPreset(int posX, int posY, const std::string& presetName, TilePlacementMode placementMode);

    /**
    * @brief Applies a preset.
    *
    * @param presetPosition Coordinates of the the top left corner of the preset.
    * @param presetName Name of the preset.
    * @param placementMode Placement mode of the tiles. Set ::UseTileDefault to use tile default.
    */
    void SetPreset(const SDL_Point& presetPosition, const std::string& presetName, TilePlacementMode placementMode);

    /**
    * @brief Applies a preset.
    *
    * @param presetJSON JSON containing the preset data.
    * @param [out] outData Pointer to the vector to put the preset save data in. If nullptr, data will not be put in.
    * Vector will NOT be cleared or created, it will add ontop of existing items.
    */
    void SetPreset(const nlohmann::json& presetJSON, std::vector<PresetSaveData>* outData = nullptr);

    /**
    * @brief Returns the condition registry of the map.
    * 
    * @return Reference to the condition registry of the map.
    */
    ConditionRegistry& GetConditionRegistry();

    /**
    * @brief Returns the facing direction of the player as stated in the map JSON.
    * 
	* @return The facing direction of the player as stated in the map JSON, or Direction::ENUM_SENTINEL_VALUE if it is not defined or declared improperly.
    */
    Direction GetMapDefaultDirection() const;

    /**
	* @brief Returns the default dialogue of the map.
    * 
	* @return The default dialogue of the map.
    */
	DialogueData GetDefaultDialogue() const;

    /**
    * @brief Checks if a given point (tile) is inside the map boundaries.
    *
    * @param x X coordinate of the point.
    * @param y Y coordinate of the point.
    *
    * @return 'true' if the point is in bounds, 'false' if otherwise.
    */
    bool IsInMapBounds(int x, int y) const;

    /**
    * @brief Checks if a given point (tile) is inside the map boundaries.
    *
    * @param point Structure containing the position of the point.
    *
    * @return 'true' if the point is in bounds, 'false' if otherwise.
    */
    bool IsInMapBounds(const SDL_Point& point) const;

    /**
    * @brief Loads the map from a JSON file.
    * 
    * @param filePath File path to the JSON file.
    */
    void LoadMapFromFile(const std::filesystem::path& filePath);

    /**
    * @brief Move operator.
    */
    Map& operator=(Map&& other) noexcept;

    void LoadMapFromFile(const std::string& filePath) = delete;
    void LoadMapFromFile(const char * filePath) = delete;
};

#endif // MAP_H_
