#include <fstream>
#include <nlohmann/json.hpp>
#include <SDL3/SDL.h>
#include <unordered_set>

#include "TileRegistry.h"

#include "Configuration.h"
#include "Assets.h"
#include "FloatUtils.h"
#include "ConstantConfiguration.h"
#include "FileLoader.h"
#include "JSONParser.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

std::vector<TileInstance> TileRegistry::tiles;
std::unordered_map<HashID, size_t> TileRegistry::tileIdToIndex;
std::unordered_map<HashID, std::string> TileRegistry::tileIdToName;

bool TileRegistry::LoadTilesFromFile(const fs::path& filePath)
{

    /*
        What this function expects:

        Note that 'root' is the main JSON object.

        {
            "tiles": [
                {
                    "name": "Grass",
                    "footprint": { "w": 1, "h": 1 },
                    "placement": "Replace",
                    "collision": [],
                    "renderParts": [
                        {
                            "src": { "x": 0, "y": 0 },
                            "offset": { "x": 0, "y": 0 },
                            "layer": "Ground"
                        }
                    ]
                },
                {
                    "name": "Tree",
                    "footprint": { "w": 1, "h": 2 },
                    "placement": "Add",
                    "audio": {
                        "steppedOn": {
                            "clip": "ui_click",
                            "playback": {
                                "volume": 0.7,
                                "fadeInMs": 0,
                                "loop": false,
                                "loopCount": 0
                            }
                        }
                    },
                    "metadata": [
                        {
                            "defaultAudio": "idle",
                            "audio": ["steppedOn"],
                            "x": 0,
                            "y": 0
                        }
                    ],
                    "animations": {
                        "idle": {
                            "loop": true,
                            "randomStart": true,
                            "frames": [
                                { "src": { "x": 4, "y": 0 }, "duration": 1.0 },
                                { "src": { "x": 4, "y": 1 }, "duration": 1.0 },
                                { "src": { "x": 4, "y": 2 }, "duration": 1.0 },
                                { "src": { "x": 4, "y": 3 }, "duration": 1.0 }
                            ]
                        },
                    },
                    "collision": [
                        { "x": 0, "y": 1, "solid": true, "blocksVision": false }
                    ],
                    "renderParts": [
                        {
                            "src": { "x": 3, "y": 4 },
                            "offset": { "x": 0, "y": 0 },
                            "layer": "Ground"
                        },
                        {
                            "src": { "x": 3, "y": 3 },
                            "offset": { "x": 0, "y": 1 },
                            "layer": "Foreground"
                        }
                    ]
                }
            ]
        }
    */

    const std::string pathStr = filePath.string();
    const char* filePathStr = pathStr.c_str();

    const std::optional<json> jsonOptional = FileLoader::LoadJSON(filePath);

    if (!jsonOptional)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Failed to open %s", filePathStr);
        return false;
    }

    const json& root = *jsonOptional;

    if (!root.is_array())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Expected array, got non-array for file: %s.", filePathStr);
        return false;
    }

    float sheetW = 0;
    float sheetH = 0;
    SDL_Texture* spriteSheet = Assets::GetSpriteSheet();
    if (!spriteSheet)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Sprite sheet in null.");
        return false;
    }

    if (!SDL_GetTextureSize(spriteSheet, &sheetW, &sheetH))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Failed GetTextureSize: %s", SDL_GetError());
        return false;
    }

    size_t index = 0;

    for (const json& tileDef : root)
    {
        TileInstance tile;

        const GameConfiguration& config = Configuration::Get();
        const int tileWidth = config.tiles.width;
        const int tileHeight = config.tiles.height;

        if (!tileDef.is_object())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Tile definution [%llu] isn't object in file: %s", index, filePathStr);
            continue;
        }

        if (!tileDef.contains("name") || !tileDef["name"].is_string())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Tile [%llu] doesn't contain 'name' or is not a string in file: %s", index, filePathStr);
            continue;
        }

        tile.name = tileDef.at("name").get<std::string>();
        tile.id = FNV1aHash::HashString(tile.name);

        if (tileIdToName.contains(tile.id))
        {
            if (tileIdToName[tile.id] == tile.name)
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Duplicate tile name [%s] in file: %s", tile.name.c_str(), filePathStr);
            }
            else
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Tile hash ID collision [%s] vs [%s] in file: %s", tile.name.c_str(), tileIdToName[tile.id].c_str(), filePathStr);
            }
            continue;
        }

        if (!tileDef.contains("footprint") || (!tileDef["footprint"].is_object() && !tileDef["footprint"].is_array() && !tileDef["footprint"].is_number()))
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Tile [%llu | %s] missing 'footprint' or is not object / array or number in file: %s. Defaulting to 1x1.", index, tile.name.c_str(), filePathStr);
        }

        if (!tileDef.contains("placement") || !tileDef["placement"].is_string())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Tile [%llu | %s] missing 'placement' in file: %s. Defaulting to 'Replace'.", index, tile.name.c_str(), filePathStr);
        }

        const std::string& placement = tileDef.value("placement", "replace");
        TilePlacementMode placementMode = TilePlacementModeFromString(placement);

        if (IsSentinel(placementMode))
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Tile [%llu | %s] placement mode from string [%s] returned sentinel in file: %s. Defaulting to 'Replace'.", index, tile.name.c_str(), placement.c_str(), filePathStr);
			placementMode = TilePlacementMode::Replace;
        }

        tile.placementMode = placementMode;
        const json fp = tileDef.value("footprint", json::object());
        tile.footprint = JSONParser::ParseJSONSize<Size>(fp, Size{ 1, 1 });

        if (tileDef.contains("audio"))
        {
            if (tileDef["audio"].is_object())
            {
                for (const auto& [key, audioJson] : tileDef["audio"].items())
                {
                    TileAudio tileAudio;

                    if (!audioJson.is_object())
                    {
                        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Audio entry [%s] for tile [%llu | %s] is not an object in file: %s.", key.c_str(), index, tile.name.c_str(), filePathStr);
                        continue;
                    }

                    if (!audioJson.contains("clip") || !audioJson["clip"].is_string())
                    {
                        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Audio entry [%s] for tile [%llu | %s] does not contain clip name in file: %s.", key.c_str(), index, tile.name.c_str(), filePathStr);
                        continue;
                    }

                    TileReactionType reactionType = TileReactionTypeFromString(key);

                    if (IsNone(reactionType))
                    {
                        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Audio entry [%s] for tile [%llu | %s] returned none for audio type from string in file: %s. Skipping", key.c_str(), index, tile.name.c_str(), filePathStr);
                        continue;
                    }

                    const std::string& clipName = audioJson.at("clip").get_ref<const std::string&>();

                    tileAudio.clipID = FNV1aHash::HashString(clipName);

                    if (audioJson.contains("playback"))
                    {
                        if (audioJson["playback"].is_object())
                        {
                            tileAudio.hasPlaybackOverride = true;
                            tileAudio.playbackOverride = JSONParser::ParseJSONAudioPlaybackOptions(audioJson["playback"]);
                        }
                        else if (!audioJson["playback"].is_null())
                        {
                            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Audio entry [%s] for tile [%llu | %s] has playback options, but is not object or null in file: %s. Skipping", key.c_str(), index, tile.name.c_str(), filePathStr);
                        }
                    }
                    else
                    {
                        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Audio entry [%s] for tile [%llu | %s] has no playback options in file: %s. To supress this, put the key and null.", key.c_str(), index, tile.name.c_str(), filePathStr);
                    }

                    tile.soundMask |= reactionType;

                    tile.audioTable[TileReactionIndexFromType(reactionType)] = std::move(tileAudio);
                }
            }
            else if (!tileDef["audio"].is_null())
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Tile [%llu | %s] has audio but is not object or null in file: %s.", index, tile.name.c_str(), filePathStr);
            }
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Tile [%llu | %s] has no audio entry in file: %s. To supress this, put the key and null.", index, tile.name.c_str(), filePathStr);
        }

        if (tileDef.contains("animations"))
        {
            if (tileDef["animations"].is_object())
            {
                for (auto& [key, animJson] : tileDef["animations"].items())
                {
                    if (!animJson.is_object())
                    {
                        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Animation entry [%s] for tile [%llu | %s] is not an object in file: %s.", key.c_str(), index, tile.name.c_str(), filePathStr);
                        continue;
                    }

                    if (!animJson.contains("loop") || !animJson["loop"].is_boolean())
                    {
                        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Animation entry [%s] for tile [%llu | %s] does not have a loop flag or is not boolean in file: %s. Defaulting to 'false'", key.c_str(), index, tile.name.c_str(), filePathStr);
                    }

                    if (!animJson.contains("randomOffset") || !animJson["randomOffset"].is_boolean())
                    {
                        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Animation entry [%s] for tile [%llu | %s] does not have a randomOffset flag or is not boolean in file: %s. Defaulting to 'false'", key.c_str(), index, tile.name.c_str(), filePathStr);
                    }

                    TileAnimationType animationType = TileReactionTypeFromString(key);

                    if (IsNone(animationType))
                    {
                        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Animation entry [%s] for tile [%llu | %s] returned nullopt for animation type from string in file: %s. Skipping.", key.c_str(), index, tile.name.c_str(), filePathStr);
                        continue;
                    }

                    tile.animationMask |= animationType;

                    AnimationClip clip;
                    clip.loopAnimationFlag = animJson.value("loop", true);
                    clip.randomOffsetFlag = animJson.value("randomOffset", false);

                    for (const json& frame : animJson["frames"])
                    {
                        if (!frame.is_object())
                        {
                            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Frame entry for animation entry [%s] for tile [%llu | %s] is not an object in file: %s.", key.c_str(), index, tile.name.c_str(), filePathStr);
                            continue;
                        }

                        if (!frame.contains("src") || (!frame["src"].is_object() && !frame["src"].is_array() && !frame["src"].is_number()))
                        {
                            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Frame entry for animation entry [%s] for tile [%llu | %s] does not contain source or is not object / array / number in file: %s. Skipping", key.c_str(), index, tile.name.c_str(), filePathStr);
                            continue;
                        }

                        float duration = 1000.0f;
                        if (frame.contains("duration") && (frame["duration"].is_number() || frame["duration"].is_object()))
                        {
                            duration = JSONParser::ParseJSONTime<float>(frame["duration"], duration);
                        }
                        else
                        {
                            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Frame entry for animation entry [%s] for tile [%llu | %s] does not contain duration or is not number / object in file: %s. Defaulting to 1 second", key.c_str(), index, tile.name.c_str(), filePathStr);
                        }

                        const json src = frame.value("src", json::object());
                        const SDL_FPoint srcPoint = JSONParser::ParseJSONPoint<SDL_FPoint>(src, { 0, 0 });

                        AnimationFrame f{
                            .src = SDL_FRect {
                                static_cast<float>(srcPoint.x * tileWidth),
                                static_cast<float>(srcPoint.y * tileHeight),
                                static_cast<float>(tileWidth),
                                static_cast<float>(tileHeight)
                            },
                            .duration = duration
                        };

                        clip.frames.push_back(std::move(f));
                    }

                    tile.animationTable[TileReactionIndexFromType(animationType)] = std::move(clip);
                }
            }
            else if (!tileDef["animations"].is_null())
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Tile [%llu | %s] has animations but is not object or null in file: %s.", index, tile.name.c_str(), filePathStr);
            }
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Tile [%llu | %s] has no animations entry in file: %s. To supress this, put the key and null.", index, tile.name.c_str(), filePathStr);
        }

        if (!tileDef.contains("renderParts") || !tileDef["renderParts"].is_array())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Tile [%llu | %s] missing 'renderParts' in file: %s. Skipping.", index, tile.name.c_str(), filePathStr);
            continue;
        }

        for (const json& rp : tileDef["renderParts"])
        {
            if (!rp.is_object())
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Render part entry for tile [%llu | %s] is not object in file: %s. Skipping.", index, tile.name.c_str(), filePathStr);
                continue;
            }

            if (!rp.contains("src") || (!rp["src"].is_object() && !rp["src"].is_array()))
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Render part entry for tile [%llu | %s] doesn't contain 'source' or is not an object / array in file: %s. Defaulting to {0, 0}", index, tile.name.c_str(), filePathStr);
            }

            if (!rp.contains("offset") || (!rp["offset"].is_object() && !rp["offset"].is_array()))
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Render part entry for tile [%llu | %s] doesn't contain 'offset' or is not object / array in file: %s. Defaulting to {0, 0}.", index, tile.name.c_str(), filePathStr);
            }

            if (!rp.contains("layer") || !rp["layer"].is_string())
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Render part entry for tile [%llu | %s] doesn't contain 'layer' or is not string in file: %s. Defaulting to 'ground'.", index, tile.name.c_str(), filePathStr);
            }

            if (!rp.contains("zBias") || !rp["zBias"].is_number())
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Render part entry for tile [%llu | %s] doesn't contain 'zbias' or is not number in file: %s. Defaulting to '0'.", index, tile.name.c_str(), filePathStr);
            }

            const json src = rp.value("src", json::object());
            const SDL_Point srcPoint = JSONParser::ParseJSONPoint<SDL_Point>(src, { 0, 0 });

            TileRenderPart part{
                .srcRect = SDL_FRect {
                    static_cast<float>(srcPoint.x * tileWidth),
                    static_cast<float>(srcPoint.y * tileHeight),
                    static_cast<float>(tileWidth),
                    static_cast<float>(tileHeight)
                }
            };

            if (FloatUtils::IsGreaterF(part.srcRect.x + part.srcRect.w, sheetW) || FloatUtils::IsGreaterF(part.srcRect.y + part.srcRect.h, sheetH))
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                    "TileRegistry.LoadTilesFromFile: Render part entry for tile [%llu | %s] has out-of-bounds source rect [%.1f, %.1f, %.1f, %.1f] (sheet size: %fx%f) in file: %s",
                    index, tile.name.c_str(), part.srcRect.x, part.srcRect.y, part.srcRect.w, part.srcRect.h, sheetW, sheetH, filePathStr);
                continue;
            }

            if (rp.contains("defaultAnimation"))
            {
                if (rp["defaultAnimation"].is_string())
                {
                    const std::string& animationStr = rp.at("defaultAnimation").get_ref<const std::string&>();
                    const TileAnimationType animationType = TileReactionTypeFromString(animationStr);

                    if (IsNone(animationType))
                    {
                        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Render part entry for tile [%llu | %s] default animation type from string [%s] returned none in file: %s.", index, tile.name.c_str(), animationStr.c_str(), filePathStr);
                    }

                    part.defaultAnimation = animationType;

                    if (!IsNone(part.defaultAnimation) && !HasFlag(tile.animationMask, part.defaultAnimation))
                    {
                        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Render part entry for tile [%llu | %s] default animation type does not appear in the tile's animation list in file: %s. Defaulting to 'None'", index, tile.name.c_str(), filePathStr);
                        part.defaultAnimation = TileAnimationType::None;
                    }

                    part.allowedAnimationsMask |= part.defaultAnimation;
                }
                else if (!rp["defaultAnimation"].is_null())
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Render part entry for tile [%llu | %s] does have a default animation but is not a string or null in file: %s.", index, tile.name.c_str(), filePathStr);
                }
            }
            else
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Render part entry for tile [%llu | %s] does not have a default animation in file: %s. To supress this, put the key and null.", index, tile.name.c_str(), filePathStr);
            }

            if (rp.contains("animations"))
            {
                if (rp["animations"].is_array())
                {
                    for (const json& animation : rp["animations"])
                    {
                        if (!animation.is_string())
                        {
                            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Render part entry for tile [%llu | %s] allowed animations' entry is not string in file: %s.", index, tile.name.c_str(), filePathStr);
                            continue;
                        }

                        const std::string& animationStr = animation.get_ref<const std::string&>();
                        const TileAnimationType animationType = TileReactionTypeFromString(animationStr);

                        if (IsNone(animationType))
                        {
                            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Render part entry for tile [%llu | %s] allowed animations' entry type from string [%s] returned none in file: %s. Skipping.", index, tile.name.c_str(), animationStr.c_str(), filePathStr);
                            continue;
                        }

                        if (!IsNone(animationType) && !HasFlag(tile.animationMask, animationType))
                        {
                            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Render part entry for tile [%llu | %s] allowed animations' entry type does not appear in the tile's animation list in file: %s. Skipping.", index, tile.name.c_str(), filePathStr);
                            continue;
                        }

                        part.allowedAnimationsMask |= animationType;
                    }
                }
                else if (!rp["animations"].is_null())
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Render part entry for tile [%llu | %s] allowed animations is not array or null in file: %s.", index, tile.name.c_str(), filePathStr);
                }
            }
            else
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Render part entry for tile [%llu | %s] does not have an array of allowed animations in file: %s. To supress this, put the key and null.", index, tile.name.c_str(), filePathStr);
            }

            const json offset = rp.value("offset", json::object());
            part.offset = JSONParser::ParseJSONPoint<SDL_Point>(offset, SDL_Point{ 0, 0 });
            // JSONs are bottom left tile based, engine is top left.
            part.offset.y = -part.offset.y;

            std::string layerStr = rp.value("layer", "Ground");
            RenderLayer layer = RenderLayerFromString(layerStr);

            if (IsSentinel(layer))
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Render part entry for tile [%llu | %s] render layer from string [%s] returned sentinel in file: %s. Defaulting to 'Ground'.", index, tile.name.c_str(), layerStr.c_str(), filePathStr);
                layer = RenderLayer::Ground;
            }

            part.layer = layer;
            part.zBias = rp.value("zBias", 0);
            tile.renderParts.push_back(std::move(part));
        }

        if (tile.renderParts.empty())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Tile [%llu | %s] has no valid render parts in file: %s. Skipping tile.", index, tile.name.c_str(), filePathStr);
            continue;
        }

        for (const TileRenderPart& part : tile.renderParts)
        {
            TileAnimationType nonDefault = part.allowedAnimationsMask & ~part.defaultAnimation;

            if (!IsNone(nonDefault))
            {
                tile.hasNonDefaultAnimations = true;
                break;
            }
        }

        if (tileDef.contains("collision"))
        {
            if (tileDef["collision"].is_array())
            {
                for (const json& c : tileDef["collision"])
                {
                    if (!c.is_object())
                    {
                        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Collision part entry for tile [%llu | %s] is not object. Skipping.", index, tile.name.c_str());
                        continue;
                    }

                    if (!c.contains("offset") || (!c["offset"].is_array() && !c["offset"].is_object()))
                    {
                        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Collision part entry for tile [%llu | %s] doesn't contain 'offset' or is not object / array. Defaulting to 0,0.", index, tile.name.c_str());
                    }

                    if (!c.contains("solid") || !c["solid"].is_boolean())
                    {
                        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Collision part entry for tile [%llu | %s] doesn't contain 'solid' or is not boolean. This may be intentional. Defaulting to false.", index, tile.name.c_str());
                    }

                    if (!c.contains("blocksVision") || !c["blocksVision"].is_boolean())
                    {
                        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Collision part entry for tile [%llu | %s] doesn't contain 'blocksVision' or is not boolean. This may be intentional. Defaulting to false.", index, tile.name.c_str());
                    }

                    TileCollisionPart part;
                    part.offset = JSONParser::ParseJSONPoint<SDL_Point>(c.value("offset", json::object()), SDL_Point{ 0, 0 });
                    // JSONs are bottom left tile based, engine is top left.
                    part.offset.y = -part.offset.y;
                    part.solidFlag = c.value("solid", false);
                    part.blocksVisionFlag = c.value("blocksVision", false);
                    tile.collisionParts.push_back(part);
                }
            }
            else if (!tileDef["collision"].is_null())
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Tile [%llu | %s] has collision entry but is not object or null in file: %s.", index, tile.name.c_str(), filePathStr);
            }
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Tile [%llu | %s] has no collisions entry in file: %s. To supress this, put key and null.", index, tile.name.c_str(), filePathStr);
        }

        if (tileDef.contains("metadata"))
        {
            if (tileDef["metadata"].is_array())
            {
                for (const json& m : tileDef["metadata"])
                {
                    if (!m.is_object())
                    {
                        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Metadata part entry for tile [%llu | %s] is not object. Skipping.", index, tile.name.c_str());
                        continue;
                    }

                    if (!m.contains("position") || (!m["position"].is_array() && !m["position"].is_object()))
                    {
                        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Metadata part entry for tile [%llu | %s] doesn't contain 'position' or is not object / array. Defaulting to {0, 0}.", index, tile.name.c_str());
                    }

                    TileMetadataPart part;

                    if (m.contains("defaultAudio"))
                    {
                        if (m["defaultAudio"].is_string())
                        {
                            const std::string& audioStr = m.at("defaultAudio").get_ref<const std::string&>();
                            TileSoundType audio = TileReactionTypeFromString(audioStr);

                            if (IsNone(audio))
                            {
                                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Metadata part entry for tile [%llu | %s] default audio type from string [%s] returned none in file: %s. Skipping", index, tile.name.c_str(), audioStr.c_str(), filePathStr);
                                continue;
                            }

                            part.defaultSound = audio;

                            if (!IsNone(part.defaultSound) && !HasFlag(tile.animationMask, part.defaultSound))
                            {
                                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Metadata part entry for tile [%llu | %s] default audio type does not appear in the tile's animation list in file: %s. Skipping", index, tile.name.c_str(), filePathStr);
                                continue;
                            }

                            part.allowedSoundsMask |= part.defaultSound;
                        }
                        else if (!m["defaultAudio"].is_null())
                        {
                            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Metadata part entry for tile [%llu | %s] does have a default audio but is not string or null in file: %s.", index, tile.name.c_str(), filePathStr);
                        }
                    }
                    else
                    {
                        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Metadata part entry for tile [%llu | %s] does not have a default audio in file: %s. To suppress this, put the key and null.", index, tile.name.c_str(), filePathStr);
                    }

                    if (m.contains("audio"))
                    {
                        if (m["audio"].is_array())
                        {
                            for (const json& audio : m["audio"])
                            {
                                if (!audio.is_string())
                                {
                                    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Metadata part entry for tile [%llu | %s] allowed audio' entry is not string in file: %s.", index, tile.name.c_str(), filePathStr);
                                    continue;
                                }

                                const std::string& audioStr = audio.get_ref<const std::string&>();
                                const TileSoundType audioType = TileReactionTypeFromString(audioStr);

                                if (IsNone(audioType))
                                {
                                    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Metadata part entry for tile [%llu | %s] allowed audio' entry type from string [%s] returned none in file: %s. Skipping.", index, tile.name.c_str(), audioStr.c_str(), filePathStr);
                                    continue;
                                }

                                if (!IsNone(audioType) && !HasFlag(tile.animationMask, audioType))
                                {
                                    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Metadata part entry for tile [%llu | %s] allowed audio' entry type does not appear in the tile's animation list in file: %s. Skipping.", index, tile.name.c_str(), filePathStr);
                                    continue;
                                }

                                part.allowedSoundsMask |= audioType;
                            }
                        }
                        else if (!m["audio"].is_null())
                        {
                            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Metadata part entry for tile [%llu | %s] allowed audio is not array or null in file: %s.", index, tile.name.c_str(), filePathStr);
                        }
                    }
                    else
                    {
                        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Metadata part entry for tile [%llu | %s] does not have an array of allowed audio in file: %s. To supress this, put the key and null.", index, tile.name.c_str(), filePathStr);
                    }

                    part.offset = JSONParser::ParseJSONPoint<SDL_Point>(m.value("position", json::object()), SDL_Point { 0, 0 });
                    // JSONs are bottom left tile based, engine is top left.
                    part.offset.y = -part.offset.y;
                    tile.metadataParts.push_back(std::move(part));
                }
            }
            else if (!tileDef["metadata"].is_null())
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Tile [%llu | %s] has metadata but is not object or null in file: %s.", index, tile.name.c_str(), filePathStr);
            }
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.LoadTilesFromFile: Tile [%llu | %s] has no metadata in file: %s. To supress this, put key and null.", index, tile.name.c_str(), filePathStr);
        }

        for (const TileMetadataPart& part : tile.metadataParts)
        {
            const TileAnimationType nonDefault = part.allowedSoundsMask & ~part.defaultSound;

            if (!IsNone(nonDefault))
            {
                tile.hasNonDefaultSounds = true;
                break;
            }
        }

        tileIdToIndex[tile.id] = index;
        tileIdToName[tile.id] = tile.name;

        tiles.push_back(std::move(tile));

        ++index;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Loaded [%llu] tiles.", tiles.size());

    return true;
}

void TileRegistry::Cleanup()
{
    tiles.clear();
    tileIdToIndex.clear();
    tileIdToName.clear();
}

TileInstance* TileRegistry::GetTileTypeByIndex(size_t index) { return (index < tiles.size()) ? &tiles[index] : nullptr; }

TileInstance* TileRegistry::GetTileTypeByName(const std::string& name)
{
    auto it = tileIdToIndex.find(FNV1aHash::HashString(name));
    return (it != tileIdToIndex.end()) ? GetTileTypeByIndex(it->second) : nullptr;
}

HashID TileRegistry::GetTileIDByName(const std::string& name)
{
    auto it = tileIdToName.find(FNV1aHash::HashString(name));
    if (it == tileIdToName.end())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.GetTileIDByName: Unknown tile '%s'", name.c_str());
        return ConstantConfiguration::invalidHashId;
    }

    return it->first;
}

int TileRegistry::GetTileIndexByName(const std::string& name)
{
    auto it = tileIdToIndex.find(FNV1aHash::HashString(name));
    if (it == tileIdToIndex.end())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.GetTileIndexByName: Unknown tile '%s'", name.c_str());
        return ConstantConfiguration::invalidTileIndex;
    }

    return static_cast<int>(it->second);
}

std::string TileRegistry::GetTileNameByIndex(int tileIndex)
{
    if (tileIndex == ConstantConfiguration::invalidTileIndex)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.GetTileNameByIndex: Invalid tile index.");
        return "";
    }

    TileInstance* tile = GetTileTypeByIndex(tileIndex);

    if (!tile)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TileRegistry.GetTileNameByIndex: Unknown tile index '%d'", tileIndex);
        return "";
    }

    return tile->name;
}
