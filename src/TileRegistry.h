#pragma once

#ifndef TILEREGISTRY_H_
#define TILEREGISTRY_H_

#include <unordered_map>
#include <string>
#include <vector>
#include <filesystem>

#include "TileInstance.h"
#include "Configuration.h"
#include "FNV1aHash.h"

/**
* @brief Static class that registers and keeps a map of tiles.
* 
* ID is the hashed name. Index is the index in the vector of tiles. Index is used to check if a tile is valid and get it for rendering.
*/
class TileRegistry
{
private:
    static std::vector<TileInstance> tiles; /// Holds the TileInstance of tiles; index -> TileInstance.
    static std::unordered_map<HashID, size_t> tileIdToIndex; /// Map of [tileId; index]
    static std::unordered_map<HashID, std::string> tileIdToName; /// Map of [tileId; name]. Used for logging and debug.

    // No instancing due to static class.
    TileRegistry() = delete;
    ~TileRegistry() = delete;

    // No copying or moving due to static class.
    TileRegistry(const TileRegistry&) = delete;
    TileRegistry& operator=(const TileRegistry&) = delete;
    TileRegistry(TileRegistry&&) = delete;
    TileRegistry& operator=(TileRegistry&&) = delete;

public:

    /**
    * @brief Loads the tiles from a file.
    * 
    * @param filePath Path to the file containing the tile configuration.
    * 
    * @return 'true' if loading was successful, 'false' if otherwise.
    */
    static bool LoadTilesFromFile(const std::filesystem::path& filePath = Configuration::Get().paths.tileFilePath);

    /**
    * @brief Cleans up the tile registry.
    */
    static void Cleanup();

    /**
    * @brief Returns the tile type of a certain index.
    *
    * @param index The index of the tile.
    *
    * @return Pointer to the tile type of the tile with the respective index or nullptr if it doesn't exist.
    */
    static TileInstance* GetTileTypeByIndex(size_t index);

    /**
    * @brief Returns the tile type of a certain tile by name.
    *
    * @param name Name of the tile.
    *
    * @return Pointer to the tile type of the tile with the respective id or nullptr if it doesn't exist.
    */
    static TileInstance* GetTileTypeByName(const std::string& name);

    /**
    * @brief Returns the tile ID of a certain tile by name.
    * 
    * @param name Name of the tile.
    * 
    * @return The ID of the tile type with the respective name or 0 if the name is invalid.
    */
    static HashID GetTileIDByName(const std::string& name);

    /**
    * @brief Returns the index of a tile type by name.
    *
    * @param name Name of the tile.
    *
    * @return The index of the tile type with said name, or the default invalid tile index if it doesn't exist.
    */
    static int GetTileIndexByName(const std::string& name);

    /**
    * @brief Returns the tile name by index.
    * 
    * @paran tileIndex Index of the tile.
    * 
    * @return The name of the tile.
    */
    static std::string GetTileNameByIndex(int tileIndex);

    static bool LoadTilesFromFile(const std::string& filePath) = delete;
    static bool LoadTilesFromFile(const char* filePath) = delete;
};

#endif // TILEREGISTRY_H_
