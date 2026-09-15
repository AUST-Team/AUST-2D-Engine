#pragma once

#ifndef GAMEMAP_H_
#define GAMEMAP_H_

#include <vector>
#include <string>
#include <unordered_set>
#include <filesystem>

#include "MapUpdateFlags.h"
#include "TilePlacementMode.h"
#include "TileSaveData.h"
#include "PresetSaveData.h"
#include "OverrideType.h"
#include "CollisionCell.h"
#include "SDL_PointOperators.h"
#include "AnimationOverrideView.h"
#include "TileAnimationOverride.h"
#include "CellAnimationRelevance.h"
#include "TileAnchorInstance.h"
#include "Direction.h"
#include "Configuration.h"
#include "DialogueData.h"
#include "TriggerOrigin.h"

class Map;
class MapLoadStrategy;
class Trigger;
class ConditionRegistry;
struct SDL_Point;
class AnimationSystem;

/*
* So...
* 
* A tile can be composed of multiple parts, both render and collision.
* The render part with the index [0,0] is called the anchor, and it's where the tile index is located on the map (where currentMap pointer leads).
*   - The collision grid is built everytime the map is loaded or a tile changes and such.
*   - The anchor coverage is a map of [PartPosition, vector<Location of anchor and anchor index>]. It is used when setting an animation override.
*     Overrides are per whole tile, but setting a override at any part, will trigger an override of the whole tile.
*   - The animation relevance is a map of [AnchorPosition, vector<Tiles at cell with X animation>]. It used to filter when setting animation overrides
*     since we can filter by the animation type.
*   - The tile animation override holds the product of the last two maps combined, by holding the actual overrides of the tiles.
*/

/**
* @brief Wrapper class for the map.
* 
* Cells can contain multiple tiles, but most function names will be '[...]Tile[...]', because I feel that it is easier to comprehend
* at a first glance, than thinking [cells have multiple tiles, but a tile is a single thing].
*/
class GameMap 
{
private:
    Map* currentMap = nullptr;   /// Pointer to the map. OWNS IT!

    std::vector<CollisionCell> collisionGrid;    /// Cache for collision (and vision blocking) tiles.

    std::vector<TileSaveData> tileChanges;      /// Vector of tile changes.
    std::vector<PresetSaveData> presetChanges;  /// Vector of preset changes.

    /**
    * @brief A map of [PartPoint, vector<TileAnchorInstance>].
    * 
    * A cell can contain multiple overlapping parts (not anchors) of a tile. 
    */
    std::unordered_map<SDL_Point, std::vector<TileAnchorInstance>> anchorCoverage;

    /// Array of map[AnchorPoint, set<tileIndex>] indexed by animation type.
    std::array<
        std::unordered_map<SDL_Point, std::unordered_set<int>>,
        TileReactionCount
    > animationRelevance;

    /// Array of map[AnchorPoint, set<tileIndex>] indexed by sound type.
    std::array<
        std::unordered_map<SDL_Point, std::unordered_set<int>>,
        TileReactionCount
    > soundRelevance;

    TileAnimationOverride tileAnimationOverrides;   /// Overrides of the tiles.

    AnimationSystem* animationSystem = nullptr;   /// Pointer to the animation system. NON OWNING!
    MapUpdateFlags pendingUpdates = MapUpdateFlags::None;   /// Pending map updates.
    bool hasAnimationSemiOverrides = false;    /// Flag if the game has animation semi overrides.
    bool hasAnimationFullOverrides = false;    /// Flag if the game has animation full overrides.

    /**
    * @brief Default constructor.
    */
    GameMap() = default;

    /**
    * @brief Default destructor.
    *
    * Calls Cleanup().
    */
    ~GameMap();

    /**
    * @brief Rebuilds the collision and vision blocking caches.
    */
    void RebuildCollisionCaches();

    /**
    * @brief Updates the full animation overrides.
    * 
    * @param deltaTime The delta time of the main game loop.
    */
    void UpdateAnimationOverrides(double deltaTime);

    /**
    * @brief Applies the animation override to an anchor.
    * 
    * NO CHECKING WILL BE DONE!
    *
    * @param anchorX The X coordinate of the anchor.
    * @param anchorY The Y coordinate of the anchor.
    * @param tileIndex The index of the anchor.
    * @param animationType The type of animation.
    * @param overrideType The type of override.
    * @param oneShot If the override should only play once and delete itself.
    * @param startTime Start time of the animation in relation to the animation system's global animation time.
    * Used for one shot semi overrides.
    */
    void ApplyAnimationOverrideAtAnchor(
        int anchorX,
        int anchorY,
        int tileIndex,
        TileAnimationType animationType,
        OverrideType overrideType,
        bool oneShot,
        float startTime
    );

    /**
    * @brief Applies the animation override to an anchor.
    * 
    * NO CHECKING WILL BE DONE!
    * 
    * @param anchorPositon The position of the anchor.
    * @param tileIndex The index of the anchor.
    * @param animationType The type of animation.
    * @param overrideType The type of override.
    * @param oneShot If the override should only play once and delete itself.
    * @param startTime Start time of the animation in relation to the animation system's global animation time.
    * Used for one shot semi overrides.
    */
    void ApplyAnimationOverrideAtAnchor(
        const SDL_Point& anchorPosition,
        int tileIndex,
        TileAnimationType animationType,
        OverrideType overrideType,
        bool oneShot,
        float startTime
    );

    /**
    * @brief Removes an animation override of an anchor.
    *
    * NO CHECKING WILL BE DONE!
    *
    * @param anchorX The X coordinate of the anchor.
    * @param anchorY The Y coordinate of the anchor.
    * @param tileIndex The index of the anchor.
    * @param overrideType The type of override.
    */
    void RemoveAnimationOverrideAtAnchor(
        int anchorX,
        int anchorY,
        int tileIndex,
        OverrideType overrideType
    );

    /**
    * @brief Removes an animation override of an anchor.
    *
    * NO CHECKING WILL BE DONE!
    *
    * @param anchorPositon The position of the anchor.
    * @param tileIndex The index of the anchor.
    * @param overrideType The type of override.
    */
    void RemoveAnimationOverrideAtAnchor(
        const SDL_Point& anchorCell,
        int tileIndex,
        OverrideType overrideType
    );

    /**
    * @brief Creates a [y * mapWidth + x] index.
    * 
    * NO CHECKING WILL BE DONE HERE!
    * 
    * @return [y * mapWidth + x] index.
    */
    size_t Index(int x, int y) const;

    // No copying or moving allowed due to singleton pattern.
    GameMap(const GameMap&) = delete;
    GameMap& operator=(const GameMap&) = delete;
    GameMap(GameMap&&) = delete;
    GameMap& operator=(GameMap&&) = delete;

public:

    /**
    * @brief Returns the created GameMap instance.
    * 
    * @return Reference to the GameMap instance.
    */
    static GameMap& GetInstance() noexcept
    {
        static GameMap instance;
        return instance;
    }

    /**
    * @brief Updates the map.
    * 
    * @param deltaTime The delta time of the main game loop.
    */
    void Update(double deltaTime);

    /**
    * @brief Cleans up the game map.
    */
    void Cleanup();

    /**
    * @brief Loads a map.
    * 
    * @param filePath Path to the file containing the map file.
    * @param loadStrategy The way to load the map. Nullptr for default loading.
    */
    void LoadMap(const std::filesystem::path& filePath, MapLoadStrategy* loadStrategy = nullptr);

    /**
    * @brief Loads a map from its name.
    *
    * @param mapName Name of the map as defined in the map file.
    * @param loadStrategy The way to load the map. Nullptr for default loading.
    */
    void LoadMapFromName(const std::string& mapName, MapLoadStrategy* loadStrategy = nullptr);
    
    /**
    * @brief Returns the map name.
    * 
    * @return The map name of the current map, or "" if there is no current map.
    */
    std::string GetMapName() const;

    /**
    * @brief Sets / adds a tile at a specified cell to a certain type.
    * 
    * @param cellX X coordinate of the cell.
    * @param cellY Y coordinate of the cell.
    * @param tileIndex The inedx of the tile.
    * @param placementMode Placement mode of the tile.
    */
    void SetTile(int cellX, int cellY, int tileIndex, TilePlacementMode placementMode);

    /**
    * @brief Sets / adds a tile at a specified cell to a certain type.
    * 
    * @param cellPosition The position of the cell.
    * @param tileIndex The index of the tile.
    * @param placementMode Placement mode of the tile.
    */
    void SetTile(const SDL_Point& cellPosition, int tileIndex, TilePlacementMode placementMode);

    /**
    * @brief Sets / adds a tile at a specified cell to a certain type.
    * 
    * @param cellX X coordinate of the cell.
    * @param cellY Y coordinate of the cell.
    * @param tileName The name of the tile.
    * @param placementMode Placement mode of the tile.
    */
    void SetTile(int cellX, int cellY, const std::string& tileName, TilePlacementMode placementMode);

    /**
    * @brief Sets / adds a tile at a specified cell to a certain type.
    * 
    * @param cellPosition The position of the cell.
    * @param tileName Name of the tile.
    * @param placementMode Placement mode of the tile.
    */
    void SetTile(const SDL_Point& cellPosition, const std::string& tileName, TilePlacementMode placementMode);

    /**
    * @brief Sets / adds a tile at a specified cell to a certain type.
    * 
    * @param tileJSON JSON containing the tile data.
    */
    void SetTile(const nlohmann::json& tileJSON);

    /**
    * @brief Returns the first tile of a specified cell.
    * 
    * @param cellX X coordinate of the cell.
    * @param cellY Y coordinate of the cell.
    * 
    * @return The ID of the respective tile or the default invalid tile id if the tile is out of bounds.
    */
    int GetTile(int cellX, int cellY) const;

    /**
    * @brief Returns the first tile of a specified cell.
    *
    * @param cellPosition The position of the cell.
    *
    * @return The ID of the tile at the specified ID, or the default invalid tile ID if it's out-of-bounds.
    */
    int GetTile(const SDL_Point& cellPosition) const;

    /**
    * @brief Returns the tiles at the specified cell.
    *
    * @param cellX X coordinate of the cell.
    * @param cellY Y coordinate of the cell.
    *
    * @return Reference to the vector containing the tiles at the specified coordinates, or an empty vector if the is no current map.
    */
    const std::vector<int>& GetTiles(int cellX, int cellY) const;

    /**
    * @brief Returns the tiles at the specified cell.
    *
    * @param cellPosition The position of the cell.
    *
    * @return Reference to the vector containing the tiles at the specified coordinates, or an empty vector if the is no current map.
    */
    const std::vector<int>& GetTiles(const SDL_Point& cellPosition) const;

    /**
    * @brief Sets a map preset.
    *
    * @param presetX The X coordinate of the top left corner of the preset.
    * @param presetY The Y coordinate of the top left corner of the preset.
    * @param presetName Name of the preset.
	* @param placementMode Placement mode of the preset. Pass TilePlacementMode::None to use tile defaults.
    */
    void SetPreset(int presetX, int presetY, const std::string& presetName, TilePlacementMode placementMode);

    /**
    * @brief Applies a preset.
    *
    * @param presetPosition Coordinates of the the top left corner of the preset.
    * @param presetName Name of the preset.
    * @param placementMode Placement mode of the preset. Pass TilePlacementMode::None to use tile defaults.
    */
    void SetPreset(const SDL_Point& presetPosition, const std::string& presetName, TilePlacementMode placementMode);

    /**
    * @brief Applies a preset.
    * 
    * @param presetJSON JSON containing the preset data.
    */
    void SetPreset(const nlohmann::json& presetJSON);

    /**
    * @brief Returns the width of the current map.
    * 
    * @return The width of the map in tiles, or 0 if no map is loaded.
    */
    int GetMapWidth() const;

    /**
    * @brief Returns the height of the current map.
    *
    * @return The height of the map in tiles, or 0 if no map is loaded.
    */
    int GetMapHeight() const;

    /**
    * @brief Returns the width of the current map.
    *
    * @return The width of the map in pixels, or 0 if no map is loaded.
    */
    int GetMapWidthPixels() const;

    /**
    * @brief Returns the height of the current map.
    *
    * @return The height of the map in pixels, or 0 if no map is loaded.
    */
    int GetMapHeightPixels() const;

    /**
    * @brief Adds a trigger to the map
    *
    * @param triggerX X coordinate of the trigger.
    * @param triggerY Y coordinate of the trigger.
    * @param trigger Pointer to the Trigger to place. OWNING!
    */
    void SetTrigger(int triggerX, int triggerY, Trigger* trigger);

    /**
    * @brief Adds a trigger to the map
    *
    * @param triggerPosition The position of the trigger.
    * @param trigger Pointer to the Trigger to place. OWNING!
    */
    void SetTrigger(const SDL_Point& triggerPosition, Trigger* trigger);

    /**
    * @brief Adds a trigger to the map.
    * 
    * @param triggerJSON JSON containing the trigger data.
    * @param origin Origin of the trigger (static or runtime).
    */
    void SetTrigger(const nlohmann::json& triggerJSON, TriggerOrigin origin = TriggerOrigin::Static);

    /**
    * @brief Return a trigger from the map from a certain cell.
    *
    * @param cellX X coordinate of the cell.
    * @param cellY Y coordinate of the cell.
    *
    * @return A vector of the trigger pointers on the cell, or an empty vector if none exist.
    */
    std::vector<Trigger *>& GetTriggers(int cellX, int cellY);

    /**
    * @brief Return a trigger from the map from a certain cell.
    *
    * @param cellPosition The position of the cell.
    *
    * @return A vector of the trigger pointers on the cell, or an empty vector if none exist.
    */
    std::vector<Trigger *>& GetTriggers(const SDL_Point& cellPosition);

    /**
    * @brief Returns all triggers from the map.
    * 
    * @return All triggers from the map.
    */
    std::vector<Trigger *> GetAllTriggers();

    /**
    * @brief Returns the condition registry of the map.
    * 
    * @return The condition registry of the map.
    */
    ConditionRegistry& GetConditionRegistry();

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
    * @brief Checks if a cell at a given position is solid or not.
    * 
    * @param cellX X coordinate of the cell.
    * @param cellY Y coordinate of the cell.
    * 
    * @return 'true' if the cell is solid, 'false' if cell is not solid or doesn't exist.
    */
    bool IsTileSolid(int cellX, int cellY) const;

    /**
    * @brief Checks if a tile at a given position is solid or not.
    *
    * @param cellPosition The position of the cell.
    *
    * @return 'true' if the tile is solid, 'false' if cell is not solid or doesn't exist.
    */
    bool IsTileSolid(const SDL_Point& cellPosition) const;

    /**
    * @brief Checks if a cell at a given position is blocking vision or not.
    * 
    * @param cellX X coordinate of the cell.
    * @param cellY Y coordinate of the cell.
    * 
    * @return 'true' if the cell blocks vision, 'false' if the cell doesn't block vision or doesn't exist.
    */
    bool IsTileBlockingVision(int cellX, int cellY) const;

    /**
    * @brief Checks if a cell at a given position is blocking vision or not.
    *
    * @param cellPosition The position of the cell.
    *
    * @return 'true' if the cell blocks vision, 'false' if the cell doesn't block vision or doesn't exist.
    */
    bool IsTileBlockingVision(const SDL_Point& cellPosition) const;

    /**
    * @brief Retruns the anchors covering a particular cell.
    * 
    * @param cellX X coordinate of the cell.
    * @param cellY Y coordinate of the cell.
    * 
    * @return Reference to the vector of anchors.
    */
    const std::vector<TileAnchorInstance>& GetAnchorsCoveringTile(int cellX, int cellY) const;

    /**
    * @brief Retruns the anchors covering a particular cell.
    *
    * @param cellPosition The position of the cell.
    * 
    * @return Reference to the vector of anchors.
    */
    const std::vector<TileAnchorInstance>& GetAnchorsCoveringTile(const SDL_Point& cellPosition) const;

    /**
    * @brief Sets an override at the specified cell. If a tile part is covering said cell, the override will be set at the anchor of the tile.
    * 
    * @param cellX X coordinate of the cell.
    * @param cellY Y coordinate of the cell.
    * @param animationType The type of animation.
    * @param overrideType The type of override to use.
    * @param oneShot If the animation should be played once then the override removed.
    * @param tileIndexFilter If the override should only be applied to certain tiles, or all on the cells. Default invalid index for all.
    */
    void SetAnimationOverride(int cellX, int cellY, TileAnimationType animationType, OverrideType overrideType, bool oneShot = true, int tileIndexFilter = ConstantConfiguration::invalidTileIndex);

    /**
    * @brief Sets an override at the specified cell. If a tile part is covering said cell, the override will be set at the anchor of the tile.
    *
    * @param cellPosition The position of the cell.
    * @param animationType The type of animation.
    * @param overrideType The type of override to use.
    * @param oneShot If the animation should be played once then the override removed.
    * @param tileIndexFilter If the override should only be applied to certain tiles, or all on the cells. Default invalid index for all.
    */
    void SetAnimationOverride(const SDL_Point& cellPosition, TileAnimationType animationType, OverrideType overrideType, bool oneShot = true, int tileIndexFilter = ConstantConfiguration::invalidTileIndex);

    /**
    * @brief Removes an override at a specified cell.
    *
    * @param cellX X coordinate of the cell.
    * @param cellY Y coordinate of the cell.
    * @param overrideType The type of override of the animation.
    * @param animationType The type of animation. 'None' for all animations.
    * @param tileIndexFilter If only certain tiles should have their override removed (if it exists). Default invalid index for all.
    */
    void RemoveAnimationOverride(int cellX, int cellY, OverrideType overrideType, TileAnimationType animationType = TileAnimationType::None, int tileIndexFilter = ConstantConfiguration::invalidTileIndex);

    /**
    * @brief Removes an / all override at a specified cell.
    *
    * @param cellPosition The position of the cell.
    * @param overrideType The type of override of the animation.
    * @param animationType The type of animation. 'None' for all animations.
    * @param tileIndexFilter If only certain tiles should have their override removed (if it exists). Default invalid index for all.
    */
    void RemoveAnimationOverride(const SDL_Point& cellPosition, OverrideType overrideType, TileAnimationType animationType = TileAnimationType::None, int tileIndexFilter = ConstantConfiguration::invalidTileIndex);

    /**
    * @brief Removes all overrides at a specified cell.
    *
    * @param cellX X coordinate of the cell.
    * @param cellY Y coordinate of the cell.
    */
    void RemoveAllAnimationOverrides(int cellX, int cellY);

    /**
    * @brief Removes all overrides at a specified cell.
    *
    * @param cellPosition The position of the cell.
    */
    void RemoveAllAnimationOverrides(const SDL_Point& cellPosition);

    /**
    * @brief Returns the override for a particular anchor.
    * An anchor is where the index of the tile is on the actual map, as an anchor can have multiple parts.
    * 
    * @param anchorX X coordinate of the anchor.
    * @param anchorY Y coordinate of the anchor.
    * @param tileIndex Index of the tile of the override.
    * 
    * @return Pointer to the animation overrides (either full, semi or both), or nullptr if none exist.
    */
    AnimationOverrideView GetAnimationOverride(int anchorX, int anchorY, int tileIndex);

    /**
    * @brief Returns the override for a particular anchor.
    * An anchor is where the index of the tile is on the actual map, as an anchor can have multiple parts.
    *
    * @param anchorPosition Position of the anchor.
    * @param tileIndex Index of the tile of the override.
    * 
    * @return Pointer to the animation override (either full, semi or both), or nullptr if there's no override.
    */
    AnimationOverrideView GetAnimationOverride(const SDL_Point& anchorPosition, int tileIndex);

    /**
    * @brief Applies the look at / away animations.
    *
    * @param playerX X coordinate of the player.
    * @param playerY Y coordinate of the player.
    * @param playerDirection The direction the player is facing.
    * @param overrideType The type of override.
    * @param oneShot 'true' if the animation should only play once and then remove itself.
    * @param range The range to check the tiles around.
    * @param tileIndexFilter If the override should only be applied to certain tiles, or all on the cells. Default invalid index for all.
    */
    void ApplyLookAnimation(
        int playerX,
        int playerY,
        Direction playerDirection,
        OverrideType overrideType,
        bool oneShot = true,
        int range = Configuration::Get().spatialBuckets.farRadius,
        int tileIndexFilter = ConstantConfiguration::invalidTileIndex
    );

    /**
    * @brief Applies the look at / away animations.
    * 
    * @param playerPosition The position of the player.
    * @param playerDirection The direction the player is facing.
    * @param overrideType The type of override.
    * @param oneShot 'true' if the animation should only play once and then remove itself.
    * @param range The range to check the tiles around.
    * @param tileIndexFilter If the override should only be applied to certain tiles, or all on the cells. Default invalid index for all.
    */
    void ApplyLookAnimation(
        const SDL_Point& playerPosition, 
        Direction playerDirection, 
        OverrideType overrideType, 
        bool oneShot = true,
        int range = Configuration::Get().spatialBuckets.farRadius,
        int tileIndexFilter = ConstantConfiguration::invalidTileIndex
    );

    /**
    * @brief Plays an audio at the specified cell.
    *
    * @param cellX X coordinate of the cell.
    * @param cellY Y coordinate of the cell.
    * @param soundType The type of animation.
    * @param tileIndexFilter If the override should only be applied to certain tiles, or all on the cell. Default invalid index for all.
    */
    void PlayAudioAt(int cellX, int cellY, TileSoundType soundType, int tileIndexFilter = ConstantConfiguration::invalidTileIndex);

    /**
    * @brief Plays an audio at the specified cell.
    *
    * @param cellPosition The position of the cell.
    * @param soundType The type of animation.
    * @param tileIndexFilter If the override should only be applied to certain tiles, or all on the cell. Default invalid index for all.
    */
    void PlayAudioAt(const SDL_Point& cellPosition, TileSoundType soundType, int tileIndexFilter = ConstantConfiguration::invalidTileIndex);

    /**
    * @brief Applies the look at / away sounds.
    *
    * @param playerX X coordinate of the player.
    * @param playerY Y coordinate of the player.
    * @param playerDirection The direction the player is facing.
    * @param range The range to check the tiles around.
    * @param tileIndexFilter If the sound should only be applied to certain tiles, or all on the cells. Default invalid index for all.
    */
    void ApplyLookSound(
        int playerX,
        int playerY,
        Direction playerDirection,
        int range = Configuration::Get().spatialBuckets.farRadius,
        int tileIndexFilter = ConstantConfiguration::invalidTileIndex
    );

    /**
    * @brief Applies the look at / away sounds.
    *
    * @param playerPosition The position of the player.
    * @param playerDirection The direction the player is facing.
    * @param range The range to check the tiles around.
    * @param tileIndexFilter If the sound should only be applied to certain tiles, or all on the cells. Default invalid index for all.
    */
    void ApplyLookSound(
        const SDL_Point& playerPosition,
        Direction playerDirection,
        int range = Configuration::Get().spatialBuckets.farRadius,
        int tileIndexFilter = ConstantConfiguration::invalidTileIndex
    );

    /**
    * @brief Returns the tile changes of the map.
    * 
    * @return The tile changes of the map.
    */
    std::vector<TileSaveData> GetTileChanges();

    /**
    * @brief Returns the preset changes of the map.
    *
    * @return The preset changes of the map.
    */
    std::vector<PresetSaveData> GetPresetChanges();

    /**
    * @brief Adds pending updates to happen next update cycle.
    * 
    * @param flags Pending updates flags to add.
    */
    void AddPendingUpdateFlags(MapUpdateFlags flags);

    /**
    * @brief Requests to interact with the adjacent tiles.
    * 
    * Updates the 'pendingUpdates' flag.
    */
    void RequestInteract();

    /**
    * @brief Notifies that the player has moved.
    * 
    * Updates the 'pendingUpdates' flag.
    */
    void NotifyPlayerMoved();

    /**
    * @brief Notifies that the player has changed their direction.
    *
    * Updates the 'pendingUpdates' flag.
    */
    void NotifyPlayerChangedDirection();

    /**
    * @brief Sets the animation system.
    * It is used for one shot animation overrides.
    * 
    * @param animationSystemPtr Pointer to the animation system.
    */
    void SetAnimationSystem(AnimationSystem* animationSystemPtr);

    /**
    * @brief Returns the dialogue data of the map.
    * 
	* @return The dialogue data of the map.
    */
    DialogueData GetDialogueData() const;

    void LoadMap(const std::string& filePath, MapLoadStrategy* loadStrategy) = delete;
    void LoadMap(const char * filePath, MapLoadStrategy* loadStrategy) = delete;
};

#endif // GAMEMAP_H_
