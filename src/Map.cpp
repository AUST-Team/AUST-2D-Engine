#include <SDL3/SDL.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <unordered_set>

#include "Map.h"

#include "TileRegistry.h"
#include "Configuration.h"
#include "GameSaveData.h"
#include "Player.h"
#include "GuardManager.h"
#include "DialogueChoiceInteractable.h"
#include "DialogueNarrationInteractable.h"
#include "StringMiscs.h"
#include "TriggerFactory.h"
#include "Trigger.h"
#include "ConstantConfiguration.h"
#include "Audio.h"
#include "MemoryTracker.h"
#include "FileLoader.h"
#include "IOMiscs.h"
#include "Unused.h"
#include "JSONParser.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

Map::Map(Map&& other) noexcept :
    conditionRegistry(std::move(other.conditionRegistry)),
    triggersByPosition(std::move(other.triggersByPosition)),
    name(std::move(other.name)),
    size(other.size),
    tiles(std::move(other.tiles)),
    playerStart(other.playerStart),
    defaultDialogue(other.defaultDialogue)
{
    other.triggersByPosition.clear();
}

Map::~Map()
{
    for (auto& [pos, triggerVec] : triggersByPosition)
    {
        for (Trigger*& trigger : triggerVec)
        {
            ENG_DELETE(trigger);
            trigger = nullptr;
        }
        triggerVec.clear();
    }

    triggersByPosition.clear();
}

void Map::Update(double deltaTime, Player* player, MapUpdateFlags pendingUpdates)
{
    UNUSED(deltaTime);
    if (pendingUpdates == MapUpdateFlags::None)
    {
        return;
    }

    triggerRegistry.ProcessTriggers(pendingUpdates, player);
    CleanupAutoDeletedTriggers();
}

void Map::PlacePlayerAtStart() const { PlacePlayerAtTile(playerStart.x, playerStart.y); }

void Map::PlacePlayerAt(float x, float y) const 
{ 
    const GameConfiguration& config = Configuration::Get();
    const int tileWidth = config.tiles.width;
    const int tileHeight = config.tiles.height;

    if (x >= static_cast<float>(size.w * tileWidth) ||
        y >= static_cast<float>(size.h * tileHeight))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.PlacePlayerAtTile: Attempted to place at an out-of-bounds tile at [%f, %f]", x, y);
        return;
    }

    Player::GetInstance()->SetPosition(x, y); 
}

void Map::PlacePlayerAt(const SDL_FPoint& point) const { PlacePlayerAt(point.x, point.y); }

void Map::PlacePlayerAtTile(int x, int y) const 
{
    if (!IsInMapBounds(x, y))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.PlacePlayerAtTile: Attempted to place at an out-of-bounds tile at [%u, %u]", x, y);
        return;
    }

    Player::GetInstance()->SetPositionTile(x, y); 
}

void Map::PlacePlayerAtTile(const SDL_Point& point) const { PlacePlayerAtTile(point.x, point.y); }

std::string Map::GetName() const { return name; }

int Map::GetTile(int x, int y) const
{
    if (!IsInMapBounds(x, y))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.GetTile: Attempted to get an out-of-bounds tile at [%u, %u]", x, y);
        return ConstantConfiguration::invalidTileIndex;
    }

    const MapCell& cell = tiles[Index(x, y)];
    if (cell.tileIndexes.empty())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.GetTile: There are no tiles at [%u, %u].", x, y);
        return ConstantConfiguration::invalidTileIndex;
    }
    return cell.tileIndexes.front();
}

int Map::GetTile(const SDL_Point& tilePosition) const { return GetTile(tilePosition.x, tilePosition.y); }

const std::vector<int>& Map::GetTiles(int x, int y) const
{
    if (!IsInMapBounds(x, y))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.GetTiles: Attempted to get an out-of-bounds tile at [%u, %u]", x, y);
        static const std::vector<int> empty;
        return empty;
    }

    return tiles[Index(x, y)].tileIndexes;
}

const std::vector<int>& Map::GetTiles(const SDL_Point& tilePosition) const { return GetTiles(tilePosition.x, tilePosition.y); }

void Map::SetTile(int x, int y, int tileIndex, TilePlacementMode placementMode)
{
	if (tileIndex == ConstantConfiguration::invalidTileIndex)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.SetTile: Attempted to set an invalid tile at [%d, %d]", x, y);
        return;
	}

	if (!IsInMapBounds(x, y))
	{
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.SetTile: Attempted to set a tile out-of-bounds at [%d, %d]", x, y);
        return;
	}

    if (IsSentinel(placementMode))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.SetTile: Attempted to set a tile with sentinel placement mode at [%d, %d]", x, y);
        return;
    }

    size_t index = Index(x, y);
    
    if (placementMode == TilePlacementMode::Remove)
    {
        std::vector<int>& vec = tiles[Index(x, y)].tileIndexes;
        vec.erase(std::remove(vec.begin(), vec.end(), tileIndex), vec.end());
        return;
    }
    else if (placementMode == TilePlacementMode::Replace)
    {
        tiles[index].tileIndexes.clear();
    }

    tiles[index].tileIndexes.push_back(tileIndex);
}

void Map::SetTile(const SDL_Point& tilePosition, int tileIndex, TilePlacementMode placementMode) { SetTile(tilePosition.x, tilePosition.y, tileIndex, placementMode); }

void Map::SetTile(int x, int y, const std::string& tileName, TilePlacementMode placementMode)
{
	int tileIndex = TileRegistry::GetTileIndexByName(tileName);

    if (tileIndex == ConstantConfiguration::invalidTileIndex)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.SetTile: Attempted to set an invalid tile at [%u, %u]", x, y);
        return;
    }

    SetTile(x, y, tileIndex, placementMode);
}

void Map::SetTile(const SDL_Point& tilePosition, const std::string& tileName, TilePlacementMode placementMode) { SetTile(tilePosition.x, tilePosition.y, tileName, placementMode); }

void Map::SetTile(const nlohmann::json& tileJSON, std::vector<TileSaveData>* outChanges)
{
    /*
		What this function expects:
        
        {
            "name": "Mountain",
		    "placement": "replace" OR null
            "positions": [
                { "x": 10, "y": 10 },
                { "x": { "range": [15, 20] }, "y": 16 },
                { "x": [5, 7, 9], "y": 7 },
                { "x": { "range": [25, 30] }, "y": { "range": [25, 30] } }
            ]
        }
    */

    if (!tileJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.SetTile: Tile JSON is not object.");
        return;
    }

    if (!tileJSON.contains("name") || !tileJSON["name"].is_string())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.SetTile: Tile JSON does not contain name or is not string.");
        return;
    }

	const std::string& tileName = tileJSON.at("name").get_ref<const std::string&>();

    if(!tileJSON.contains("positions") || (!tileJSON["positions"].is_array()))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.SetTile: Tile JSON missing 'positions' for tile [%s] or value is not an array.", tileName.c_str());
        return;
	}

    TilePlacementMode placementMode = TilePlacementMode::UseTileDefault;
    if (tileJSON.contains("placement"))
    {
        if (tileJSON["placement"].is_string() )
        {
            const std::string& placementStr = tileJSON.at("placement").get_ref<const std::string&>();
            placementMode = TilePlacementModeFromString(placementStr);

            if (IsSentinel(placementMode))
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.SetTile: Tile JSON has invalid 'placement' [%s] for tile [%s]. Defaulting to \"default\"", placementStr.c_str(), tileName.c_str());
                placementMode = TilePlacementMode::UseTileDefault;
            }
        }
        else if (!tileJSON["placement"].is_null())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.SetTile: Tile JSON has 'placement' for tile [%s] but value is not a string/null", tileName.c_str());
        }
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.SetTile: Tile JSON missing 'placement' for tile [%s] or value is not a string/null. Defaulting to \"tile defaults\". To supress this warning, put the key with null value.", tileName.c_str());
    }

    const int tileIndex = TileRegistry::GetTileIndexByName(tileName);
    if (tileIndex == ConstantConfiguration::invalidTileIndex)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.SetTile: Unknown tile name [%s].", tileName.c_str());
        return;
    }

    std::unordered_set<size_t> tileSet;

    for (const json& pos : tileJSON["positions"])
    {
        if (!pos.is_object())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.SetTile: Position is not object for tile name [%s].", tileName.c_str());
            continue;
        }

        const std::vector<int>& xs = ResolvePositions(pos, "x", size.w);
        const std::vector<int>& ys = ResolvePositions(pos, "y", size.h);

        if (xs.empty() || ys.empty())
        {
            continue;
        }

        for (int y : ys)
        {
            for (int x : xs)
            {
                tileSet.insert(Index(x, y));
            }
        }
    }

    std::vector<TileSaveData> saveData;
    saveData.reserve(tileSet.size());

    for (const size_t& encoded : tileSet)
    {
        const int x = static_cast<int>(encoded % size.w);
        const int y = static_cast<int>(encoded / size.w);

        if (outChanges)
        {
            outChanges->push_back(TileSaveData{
                .position = {x, y},
                .tileIndex = tileIndex,
                .mode = placementMode
                });
        }

        SetTile(x, y, tileIndex, placementMode);
    }

    tileSet.clear();
}

int Map::GetWidth() const { return size.w; }

int Map::GetHeight() const { return size.h; }

void Map::Resize(Size newSize)
{
    std::vector<MapCell> newCells;
    newCells.resize(static_cast<size_t>(newSize.w) * newSize.h);

    const int minW = std::min(size.w, newSize.w);
    const int minH = std::min(size.h, newSize.h);

    for (int y = 0; y < minH; ++y)
    {
        for (int x = 0; x < minW; ++x)
        {
            newCells[static_cast<size_t>(y) * newSize.w + x] = tiles[Index(x, y)];
        }
    }

    tiles.swap(newCells);
    size = newSize;
}

void Map::SetTrigger(int x, int y, Trigger* trigger)
{
    if (!IsInMapBounds(x, y))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.SetTrigger: Attempted to set an out-of-bounds trigger at [%u, %u]", x, y);
        return;
    }

    if (!trigger)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.SetTrigger: Attempted to set a null trigger at [%u, %u]", x, y);
        return;
    }

    if (!triggerRegistry.RegisterTrigger(trigger))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.SetTrigger: Trigger [%s] failed to get registered in trigger registry at [%u, %u]", trigger->GetTriggerName().c_str(), x, y);
        ENG_DELETE(trigger);
        return;
    }

    SDL_Point key { x, y };
    triggersByPosition[key].push_back(trigger);
}

void Map::SetTrigger(const SDL_Point& tilePosition, Trigger* trigger) { SetTrigger(tilePosition.x, tilePosition.y, trigger); }

Trigger* Map::SetTrigger(const nlohmann::json& triggerJSON)
{
    /*
        What this function expects:

        Note that triggerJSON is the default JSON object.

        {
            "id": "triggerID"
            "position": { "x": 11, "y": 11 },

            "flags": {
                "repeatable": true,
                "autoDelete": false
            },

            "conditions": {
                "or": [
                    {"type": "interact"},
                    {"type": "steppedOn"},
                    {"and": [
                        {"type": "proximity", "range": 4},
                        {"type": "facing", "facing": "away"}
                    ]}
                ]
            },

            "interactable": {
                "type": "dialogueNarration",
                "params": {
                    "lines": [
                        "First Line",
                        "Second Line"
                    ],
                    "onEnd": {
                        "event": "eventX",
                        "params": {
                            <eventXparams>
                        }
                    }
                }
            }
        }
    
    */

    if (!triggerJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.SetTrigger: Trigger JSON is not object.");
        return nullptr;
    }

    Trigger* triggerPtr = TriggerFactory::CreateTriggerFromJSON(triggerJSON, conditionRegistry);

    const SDL_Point pos = triggerPtr->GetPosition();

    if (!triggerPtr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapTriggers: Trigger factory created null trigger at [x = %d, y = %d].", pos.x, pos.y);
        return nullptr;
    }

    SetTrigger(pos, triggerPtr);

    return triggerPtr;
}

std::vector<Trigger *>& Map::GetTriggers(int x, int y)
{
    static std::vector<Trigger*> empty;

    if (!IsInMapBounds(x, y))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.SetTrigger: Attempted to get out-of-bounds triggers at [%u, %u]", x, y);
        return empty;
    }

    auto it = triggersByPosition.find(SDL_Point { x, y });
    return (it != triggersByPosition.end()) ? it->second : empty;
}

std::vector<Trigger*>& Map::GetTriggers(const SDL_Point& tilePosition) { return GetTriggers(tilePosition.x, tilePosition.y); }

std::vector<Trigger*> Map::GetAllTriggers()
{
    std::vector<Trigger*> triggers;

    for (auto& [pos, triggerVec] : triggersByPosition)
    {
        for (Trigger* trigger : triggerVec)
        {
            triggers.push_back(trigger);
        }
    }

    return triggers;
}

Trigger* Map::GetTriggerById(uint64_t triggerID) { return triggerRegistry.GetTriggerById(triggerID); }

void Map::RemoveTrigger(Trigger* trigger) { triggerRegistry.RemoveTrigger(trigger); }

void Map::SetPreset(int posX, int posY, const std::string& presetName, TilePlacementMode placementMode)
{
    /*
        What this function expects:

        Note that presetJSON is the default JSON array.

        [
            ["skip", "Soil", "Forest", null],
            ["Forest", "Water", "Water", "Soil"],
            ["Soil", "Water", "Water", "Forest"],
            ["X", "Soil", "Soil", "X"]
        ]
    */

    const fs::path filePath = Configuration::Get().paths.presetDirectory / IOMiscs::MakeCrossPlatformPath(StringMiscs::ToLower(presetName) + ".json");

    const std::optional<json> jsonOptional = FileLoader::LoadJSON(filePath);

    if (!jsonOptional)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.SetPreset: Could not open preset [%s]", presetName.c_str());
        return;
    }

    const json& presetJson = *jsonOptional;

    if (!presetJson.is_array())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.SetPreset: Preset [%s] is not an array.", presetName.c_str());
        return;
    }

    for (int y = 0; y < presetJson.size(); ++y)
    {
        if (!presetJson[y].is_array())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.SetPreset: Preset [%s] is not a matrix / the [%u] row is not an array.", presetName.c_str(), y);
            continue;
        }

        for (int x = 0; x < presetJson[y].size(); ++x)
        {
            const json& tileName = presetJson[y][x];

            if (tileName.is_null() || tileName == "skip" || tileName == "X")
            {
                continue;
            }

            const int tileIndex = TileRegistry::GetTileIndexByName(tileName);

            if (tileIndex == ConstantConfiguration::invalidTileIndex)
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.SetPreset: Unknown tile name in preset: %s", tileName.get<std::string>().c_str());
                continue;
            }

            TilePlacementMode mode = IsSentinel(placementMode) || placementMode == TilePlacementMode::UseTileDefault ?
                TileRegistry::GetTileTypeByIndex(tileIndex)->placementMode : placementMode;

            SetTile(posX + x, posY + y, tileIndex, mode);
        }
    }
}

void Map::SetPreset(const SDL_Point& presetPosition, const std::string& presetName, TilePlacementMode placementMode) { SetPreset(presetPosition.x, presetPosition.y, presetName, placementMode); }

void Map::SetPreset(const nlohmann::json& presetJSON, std::vector<PresetSaveData>* outData)
{
    /*
        What this function expects:

        Note that presetJSON is the default JSON object.

        {
            "name": "Lake",
            "position": { "x": 25, "y": 2 },
            "placement": "Add"
        }


        {
            "name": "Lake",
            "position": { "x": 25, "y": 2 },
            "placement": null // This will use the tile's defaults
        }
    
    */

    if (!presetJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.SetPreset: Preset is not object.");
        return;
    }

    if (!presetJSON.contains("name") || !presetJSON["name"].is_string())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.SetPreset: Preset does not contain name or is not string.");
        return;
    }

    const std::string& presetName = presetJSON.at("name").get_ref<const std::string&>();

    if (!presetJSON.contains("position") || (!presetJSON["position"].is_object() && !presetJSON["position"].is_array() && !presetJSON["position"].is_number()))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.SetPreset: Preset does not contain position or is not array for preset [%s]. Defaulting to {0, 0}.", presetName.c_str());
    }

    TilePlacementMode placementMode = TilePlacementMode::UseTileDefault;
    if (presetJSON.contains("placement"))
    {
        if (presetJSON["placement"].is_string())
        {
            placementMode = TilePlacementModeFromString(presetJSON.at("placement").get_ref<const std::string&>());

            if (IsSentinel(placementMode))
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.SetPreset: Preset JSON has invalid 'placement' for preset [%s]. Defaulting to \"default\"", presetName.c_str());
                placementMode = TilePlacementMode::UseTileDefault;
            }
        }
        else if (!presetJSON["placement"].is_null())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.SetPreset: Preset JSON has 'placement' for preset [%s] but value is not a string/null", presetName.c_str());
        }
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.SetPreset: Preset JSON missing 'placement' for preset [%s] or value is not a string/null. Defaulting to \"tile defaults\". To supress this warning, put the key with null value.", presetName.c_str());
    }

    const SDL_Point position = JSONParser::ParseJSONPoint<SDL_Point>(presetJSON.value("position", json::object()), SDL_Point{ 0, 0 });

    if (outData)
    {
        outData->push_back(PresetSaveData{
            .name = presetName,
            .position = position,
            .mode = placementMode
        });
    }

    SetPreset(position, presetName, placementMode);
}

ConditionRegistry& Map::GetConditionRegistry() { return conditionRegistry; }

Direction Map::GetMapDefaultDirection() const { return playerFacingDirection; }

DialogueData Map::GetDefaultDialogue() const { return defaultDialogue; }

bool Map::IsInMapBounds(int x, int y) const
{
    return (x >= 0 && y >= 0 && x < size.w && y < size.h);
}

bool Map::IsInMapBounds(const SDL_Point& point) const { return IsInMapBounds(point.x, point.y); }

// I/O defined logic = most checking possible.
void Map::LoadMapFromFile(const fs::path& filePath)
{
    /*
        What this function expects:

        A whole map_X.json
    */

    std::optional<json> jsonOptional = FileLoader::LoadJSON(filePath);

    if (!jsonOptional)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapFromFile: Could not open file \'%s\'", filePath.string().c_str());
        return;
    }

    const json& map = *jsonOptional;

    LoadMapDetails(map);
    LoadPlayerDetails(map);
    LoadMapTiles(map);
    LoadMapGuards(map);
    LoadMapTriggers(map);
    LoadMapPresets(map);
}

Map& Map::operator=(Map&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    for (auto& [pos, vec] : triggersByPosition)
    {
        for (Trigger* t : vec)
        {
            ENG_DELETE(t);
        }
    }

    conditionRegistry = std::move(other.conditionRegistry);
    triggersByPosition = std::move(other.triggersByPosition);
    name = std::move(other.name);
    size = other.size;
    tiles = std::move(other.tiles);
    playerStart = other.playerStart;
    defaultDialogue = other.defaultDialogue;

    other.triggersByPosition.clear();

    return *this;
}

void Map::LoadMapDetails(const nlohmann::json& mapDetailsJSON)
{
    /*
        What this function expects:

        Note that 'mapDetailsJSON' is the default JSON object

        {          
            "mapId": 0,
            "name": "DebugMap",
            "dimensions": [50, 50],
            "music": "map1Music",

            "player": { 
                "playerStart": { "x": 0, "y": 1 },
                "playerFacing": "left"
            },

            "defaultDialogue": "playerCaughtNode",

            "baseTile": "Grass"
        }
    */

    if (!mapDetailsJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapDetails: Expected object, got non-object.");
        return;
    }

    if (!mapDetailsJSON.contains("name") || !mapDetailsJSON["name"].is_string())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapDetails: JSON does not contain name or is not string.");
        return;
    }

    name = mapDetailsJSON.at("name").get<std::string>();

    if (!mapDetailsJSON.contains("baseTile") || !mapDetailsJSON["baseTile"].is_string())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapDetails: JSON for map [%s] does not contain base tile or is not a string.", name.c_str());
        return;
    }

    if (!mapDetailsJSON.contains("dimensions") || (!mapDetailsJSON["dimensions"].is_array() && mapDetailsJSON["dimensions"].is_object()))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapDetails: JSON for map [%s] does not contain map dimensions or is not an array, or does not contain both values. Defaulting to {50, 50}.", name.c_str());
    }

    Size mapSize = JSONParser::ParseJSONSize<Size>(mapDetailsJSON.value("dimensions", json::array({ 50, 50 })), Size{ 50, 50 });
    Resize(mapSize);

    if (mapDetailsJSON.contains("music") && mapDetailsJSON["music"].is_string())
    {
        const std::string& musicName = mapDetailsJSON.at("music").get_ref<const std::string&>();
        Audio::Play(musicName);
    }
    else if(!mapDetailsJSON["music"].is_null())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapDetails: JSON for map [%s] contains music, but is not string or null.", name.c_str());
    }

    DialogueData data{
        .data = json {
            {"lines", json::array({"If you see this, something has went terrible wrong, or the mapper forgot to declare a default dialogue."})}
        },
        .type = DialogueType::Narration
    };

    if(!mapDetailsJSON.contains("defaultDialogue") || (!mapDetailsJSON["defaultDialogue"].is_string() && !mapDetailsJSON["defaultDialogue"].is_object()))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapDetails: JSON for map [%s] does not contain default dialogue or is not a string or object. Defaulting to a generic dialogue.", name.c_str());
    }
    else
    {
		data = JSONParser::ParseJSONDialogueData(mapDetailsJSON["defaultDialogue"], data);
	}

    defaultDialogue = data;

    const std::string& baseTileName = mapDetailsJSON.at("baseTile").get_ref<const std::string&>();

    const int baseTileIndex = TileRegistry::GetTileIndexByName(baseTileName);
    if (baseTileIndex == ConstantConfiguration::invalidTileIndex)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapDetails: Base tile [%s] not found for map [%s].", baseTileName.c_str(), name.c_str());
        return;
    }

    for (int y = 0; y < size.h; ++y)
    {
        for (int x = 0; x < size.w; ++x)
        {
            tiles[Index(x, y)].tileIndexes.clear();
            tiles[Index(x, y)].tileIndexes.push_back(baseTileIndex);
        }
    }
}

void Map::LoadMapTiles(const nlohmann::json& mapTilesJSON)
{
    /*
        What this function expects:

        Note that 'mapTilesJSON' is the default JSON object

        {
            "tiles": [
                {
                    "name": "Mountain",
                    "positions": [
                        { "x": 10, "y": 10 },
                        { "x": 12, "y": 12 },

                        { "x": { "range": [15, 20] }, "y": 16 },

                        { "x": [5, 7, 9], "y": 7 },

                        { "x": { "range": [25, 30] }, "y": { "range": [25, 30] } }
                    ]
                },
                {
                    "name": "Water",
                    "placement": "Add",
                    "positions": [{ "x": 1, "y": 1 }]
                }
            ],
        }
    */

    if(!mapTilesJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapTiles: Expected object, got non-object.");
        return;
    }

    if (!mapTilesJSON.contains("tiles") || !mapTilesJSON["tiles"].is_array())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapTiles: Tiles object is missing or not an array.");
        return;
    }

    for (const json& tile : mapTilesJSON["tiles"]) 
    {
        if (!tile.is_object())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapTiles: Tile entry is not an object. Skipping.");
            continue;
        }
        
		SetTile(tile);
    }
}

void Map::LoadMapGuards(const nlohmann::json& mapGuardsJSON)
{
    /*
        What this function expects (example):

        Note the 'mapGuardsJSON' is the default JSON object.

        {
            "guards": [
                {
                    "type": "patrol",
                    "spawn": { "x": 1, "y": 1 },
                    "path": [
                        { "x": 1, "y": 1 },
                        { "x": 8, "y": 8 },
                        { "x": 3, "y": 3 },
                        { "x": 1, "y": 3 }
                    ],
                    "facing": "Left",
                    "dialogueOverride": "nodeName"
                },
                {
                    "type": "idle",
                    "spawn": { "x": 5, "y": 5 },
                    "facing": "Down",
                    "dialogueOverride": null OR object
                }
            ]
        }
    */

    if (!mapGuardsJSON.contains("guards") || !mapGuardsJSON["guards"].is_array())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapGuards: Guard object is missing or not an array.");
        return;
    }

    for (const json& g : mapGuardsJSON["guards"])
    {
        if (!g.is_object())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapGuards: Guard entry is not an object.");
            continue;
        }

        if (!g.contains("spawn") || (!g["spawn"].is_object() && !g["spawn"].is_number() && !g["spawn"].is_array()))
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapGuards: Guard doesn't contain spawn or is not an object. Defaulting to {0, 0}.");
        }

        const json pos = g.value("spawn", json::object());
        const SDL_Point gPos = JSONParser::ParseJSONPoint(pos, SDL_Point { 0, 0 });

        std::vector<SDL_Point> patrol;

        if (g.contains("path"))
        {
            if (!g["path"].is_array())
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapGuards: Guard patrol path is not an array.");
                continue;
            }

            patrol.clear();
            for (const json& pt : g["path"])
            {
                if (!pt.is_object() && !pt.is_array() && !pt.is_number())
                {
                    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapGuards: Guard path entry is not an object / array / number.");
                    continue;
                }

                patrol.push_back(JSONParser::ParseJSONPoint<SDL_Point>(pt));
            }
        }

        if (!g.contains("facing") || !g["facing"].is_string())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapGuards: Guard starting facing direction is not a string. Defaulting to Left");
        }

        DialogueData dialogueOverride {
            .type = GetSentinel<DialogueType>()
        };

        if (g.contains("dialogueOverride"))
        {
            if (g["dialogueOverride"].is_object() || g["dialogueOverride"].is_string())
            {
                dialogueOverride = JSONParser::ParseJSONDialogueData(g["dialogueData"], dialogueOverride);
            }
            else if(!g["dialogueOverride"].is_null())
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapGuards: Guard has \"dialogueOverride\", but is not object, string or null.");
            }
            
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapGuards: Guard has no \"dialogueOverride\". To supress this, put the key and null.");
        }

        const std::string facingString = g.value("facing", "left");

        Direction facingDirection = DirectionFromString(facingString);

        if (IsSentinel(facingDirection))
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapGuards: Facing string [%s] returned sentinel value.", facingString.c_str());
            facingDirection = Direction::Left;
        }

        Guard* guard = nullptr;

        if (patrol.empty())
        {
            guard = ENG_NEW(Guard, gPos, facingDirection);
        }
        else
        {
            guard = ENG_NEW(Guard, gPos, patrol, facingDirection);
        }

        if (!guard)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapGuards: Guard pointer is null.");
            return;
        }

        if (!IsSentinel(dialogueOverride.type))
        {
            guard->SetCaughtDialogueData(dialogueOverride);
        }

        GuardManager::GetInstance().RegisterGuard(guard);
    }
}

void Map::LoadMapTriggers(const nlohmann::json& mapTriggersJSON)
{
    /*
        What this function expects (example):

        Note that 'mapTriggersJSON' is the default JSON object.

        {
            "triggers": [
                {
                    "id": "triggerID"
                    "position": { "x": 11, "y": 11 },

                    "flags": {
                        "repeatable": true,
                        "autoDelete": false
                    },

                    "conditions": {
                        "or": [
                            {"type": "interact"},
                            {"type": "steppedOn"},
                            {"and": [
                                {"type": "proximity", "range": 4},
                                {"type": "facing", "facing": "away"}
                            ]}
                        ]
                    },

                    "events": {
                        "startDialogue": {
                            "type": "narration",
                            "lines": [ "First Line", "Second Line" ],
                            "onEnd": [<events>] or NULL
                        }
                    }
                }
            ]
        }
    */

    if (!mapTriggersJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapTriggers: Expected object, got non-object.");
        return;
    }

    if (!mapTriggersJSON.contains("triggers") || !mapTriggersJSON["triggers"].is_array())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapTriggers: Triggers object is missing or not an array.");
        return;
    }

    for (const json& trigger : mapTriggersJSON["triggers"])
    {
        if (!trigger.is_object())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapTriggers: Trigger entry is not object. Skipping.");
            continue;
        }

        SetTrigger(trigger);
    }
}

void Map::LoadMapPresets(const nlohmann::json& mapPresetsJSON)
{
    /*
        What this function expects (example):

        {          
            "presets": [
                {
                    "name": "Lake",
                    "position": { "x": 25, "y": 2 },
					"placement": "Add"
                }
            ]
        }
    */

    if (!mapPresetsJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapPresets: Expected object, got non-object.");
        return;
    }

    if (!mapPresetsJSON.contains("presets") || !mapPresetsJSON["presets"].is_array())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapPresets: Presets missing or is not an array.");
        return;
    }

    for (const json& preset : mapPresetsJSON["presets"]) 
    {
        if (!preset.is_object())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadMapPresets: Preset entry is not object. Skipping");
            continue;
        }

        SetPreset(preset);
    }
}

void Map::LoadPlayerDetails(const nlohmann::json& playerDetailsJSON)
{
    /*
        What this function expects (example):
        
        Note that 'playerDetailsJSON' is the default JSON object.
        
        {
            "player": {
                "playerStart": { "x": 0, "y": 1 },
                "playerFacing": "left"
            },
        }
        
    */

    if (!playerDetailsJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadPlayerDetails: Expected object, got non-object.");
        return;
    }

    if (!playerDetailsJSON.contains("player") || !playerDetailsJSON["player"].is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadPlayerDetails: Player object missing or is not object.");
        return;
    }

    const json& player = playerDetailsJSON["player"];

    if (!player.contains("playerStart") && (!player["playerStart"].is_object() || !player["playerStart"].is_array() || !player["playerStart"].is_number()))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadPlayerDetails: JSON does not contain player start or is not an object. Defaulting to {0, 0}.");
    }

    playerStart = JSONParser::ParseJSONPoint<SDL_Point>(player.value("playerStart", json::object()), SDL_Point{ 0, 0 });

    if (!player.contains("playerFacing") || !player["playerFacing"].is_string())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadPlayerDetails: Player starting facing direction is not a string. Defaulting to Left");
    }

    const std::string facingString = player.value("playerFacing", "left");

    playerFacingDirection = DirectionFromString(facingString);

    if (IsSentinel(playerFacingDirection))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.LoadPlayerDetails: Facing string [%s] returned sentinel value.", facingString.c_str());
        playerFacingDirection = Direction::Left;
    }
}

void Map::CleanupAutoDeletedTriggers()
{
    for (auto& [pos, vec] : triggersByPosition)
    {
        std::erase_if(vec, [&](Trigger* t) {
            if (t && t->ShouldAutoDelete())
            {
                triggerRegistry.RemoveTrigger(t);
                ENG_DELETE(t);
                return true;
            }
            return false;
        });
    }
}

std::vector<int> Map::ResolvePositions(const json& posJSON, const std::string& key, int max)
{
    /*
        What this function expects:
    
        Note that 'posJSON' is the default JSON object.

        {
            "key": number
        }

        {
            "key": [number1, number2, ...]
        }

        { 
            "key": { "range": [start, end] }
        }
    */

    std::vector<int> result;

    if (!posJSON.is_object())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.ResolvePositions: Expected object, got non-object.");
        return result;
    }

    if (!posJSON.contains(key))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.ResolvePositions: Position object doesn't contain the [%s] key.", key.c_str());
        return result;
    }

    const json& v = posJSON[key];

    const std::function<void(int)> push = [&](int value) {
        if (value > 0 && value < max)
        {
            result.push_back(value);
        }
    };

    // "x": 5
    if (v.is_number_unsigned())
    {
        push(v.get<int>());
    }
    // "x": [1, 3, 7]
    else if (v.is_array())
    {
        for (const json& e : v)
        {
            if (!e.is_number_unsigned())
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.ResolvePositions: [list of values] Value is not unsigned for [%s] key.", key.c_str());
                continue;
            }

            push(e.get<int>());
        }
    }
    // "x": { "range": [start, end] }
    else if (v.is_object() && v.contains("range"))
    {
        const json& r = v["range"];
        if (!r.is_array() || r.size() != 2 ||
            !r[0].is_number_unsigned() ||
            !r[1].is_number_unsigned())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Map.ResolvePositions: [range] Range is not array, has more than 2 elements, or one or both is not unsigned for [%s] key.", key.c_str());
            return result;
        }

        int start = r.at(0).get<int>();
        int end = r.at(1).get<int>();
        if (start > end)
        {
            std::swap(start, end);
        }

        for (int i = start; i <= end && i < max; ++i)
        {
            result.push_back(i);
        }
    }

    return result;
}

size_t Map::Index(int x, int y) const { return static_cast<size_t>(y) * size.w + x; }
