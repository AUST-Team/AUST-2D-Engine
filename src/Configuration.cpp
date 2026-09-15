#include <fstream>
#include <SDL3/SDL_log.h>

#include "Configuration.h"
#include "ModManager.h"
#include "FileLoader.h"
#include "JSONParser.h"
#include "IOMiscs.h"

GameConfiguration Configuration::config;

using json = nlohmann::json;
namespace fs = std::filesystem;

const GameConfiguration& Configuration::Get() { return config; }

bool Configuration::LoadFromFile(const fs::path& filePath)
{
    /*
        What this function expects:

        A 'configuration.json' file.
    */

    const std::optional<json> jsonOptional = FileLoader::LoadJSON(filePath);

    if (!jsonOptional)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Configuration.LoadFromFile: Failed to load JSON at: %s.", filePath.string().c_str());
        return false;
    }

    LoadConfigurationFromJSON(*jsonOptional);

    return true;
}

void Configuration::LoadConfigurationFromJSON(const json& configurationJSON)
{
    if (configurationJSON.contains("window"))
    {
        const json& w = configurationJSON["window"];
        config.window.width = w.value("width", config.window.width);
        config.window.height = w.value("height", config.window.height);
        config.window.iconFileName = w.value("iconFileName", config.window.iconFileName);
        config.window.name = w.value("name", config.window.name);
    }

    if (configurationJSON.contains("app"))
    {
        const json& a = configurationJSON["app"];
        config.app.targetFPS = a.value("targetFPS", config.app.targetFPS);
        config.app.frameDelayMs = 1000.0 / config.app.targetFPS;
    }

    if (configurationJSON.contains("tiles"))
    {
        const json& t = configurationJSON["tiles"];
        config.tiles.width = t.value("width", config.tiles.width);
        config.tiles.height = t.value("height", config.tiles.height);
        config.tiles.maxScale = t.value("maxScale", config.tiles.maxScale);
        config.tiles.minScale = t.value("minScale", config.tiles.minScale);
        config.tiles.step = t.value("step", config.tiles.step);
    }

    if (configurationJSON.contains("character"))
    {
        const json& c = configurationJSON["character"];
        config.character.width = c.value("width", config.character.width);
        config.character.height = c.value("height", config.character.height);
        config.character.speed = c.value("speed", config.character.speed);
    }

    if (configurationJSON.contains("player"))
    {
        const json& p = configurationJSON["player"];
        config.player.maxTimesCaught = p.value("maxTimesCaught", config.player.maxTimesCaught);
    }

    if (configurationJSON.contains("guard"))
    {
        const json& g = configurationJSON["guard"];

        if (g.contains("speed"))
        {
            config.guard.patrolSpeed = g["speed"].value("patrol", config.guard.patrolSpeed);
            config.guard.chaseSpeed = g["speed"].value("chase", config.guard.chaseSpeed);
        }

        if (g.contains("vision"))
        {
            const json& v = g["vision"];

            if (v.contains("base"))
            {
                const json& base = v["base"];
                config.guard.vision.base.centralAngle = base.value("centralAngle", config.guard.vision.base.centralAngle);
                config.guard.vision.base.coneAngle = base.value("coneAngle", config.guard.vision.base.coneAngle);
                config.guard.vision.base.peripheralAngle = base.value("peripheralAngle", config.guard.vision.base.peripheralAngle);
                config.guard.vision.base.centralRange = base.value("centralRange", config.guard.vision.base.centralRange);
                config.guard.vision.base.coneRange = base.value("coneRange", config.guard.vision.base.coneRange);
                config.guard.vision.base.peripheralRange = base.value("peripheralRange", config.guard.vision.base.peripheralRange);
            }

            if (v.contains("patrol"))
            {
                const json& p = v["patrol"];
                config.guard.vision.patrol.additionalAngle = p.value("additionalAngle", config.guard.vision.patrol.additionalAngle);
                config.guard.vision.patrol.additionalTiles = p.value("additionalTiles", config.guard.vision.patrol.additionalTiles);
            }

            if (v.contains("chase"))
            {
                const json& c = v["chase"];
                config.guard.vision.chase.additionalAngle = c.value("additionalAngle", config.guard.vision.chase.additionalAngle);
                config.guard.vision.chase.additionalTiles = c.value("additionalTiles", config.guard.vision.chase.additionalTiles);
            }

            if (v.contains("search"))
            {
                const json& s = v["search"];
                config.guard.vision.search.additionalAngle = s.value("additionalAngle", config.guard.vision.search.additionalAngle);
                config.guard.vision.search.additionalTiles = s.value("additionalTiles", config.guard.vision.search.additionalTiles);
            }
        }

        if (g.contains("timings"))
        {
            const json& t = g["timings"];
            config.guard.timings.alertDuration = JSONParser::ParseJSONTime<float>(t["alertDuration"], config.guard.timings.alertDuration);
            config.guard.timings.aggressionReduction = JSONParser::ParseJSONTime<float>(t["aggressionReduction"], config.guard.timings.aggressionReduction);
            config.guard.timings.visionReduction = JSONParser::ParseJSONTime<float>(t["visionReduction"], config.guard.timings.visionReduction);
            config.guard.timings.patrolPause = JSONParser::ParseJSONTime<float>(t["patrolPause"], config.guard.timings.patrolPause);
        }
    }

    if (configurationJSON.contains("guardSearch"))
    {
        const json& gs = configurationJSON["guardSearch"];
        config.guardSearch.followTiles = gs.value("followTiles", config.guardSearch.followTiles);
        config.guardSearch.randomSearchTime = JSONParser::ParseJSONTime<float>(gs["randomSearchTime"], config.guardSearch.randomSearchTime);
        config.guardSearch.aggressionRandomTimeMultiplier = gs.value("aggressionRandomTimeMultiplier", config.guardSearch.aggressionRandomTimeMultiplier);
    }

    if (configurationJSON.contains("ui"))
    {
        const json& u = configurationJSON["ui"];

        if (u.contains("colors"))
        {
            const json& c = u["colors"];
            config.ui.background = JSONParser::ParseJSONColor(c["background"], config.ui.background);
            config.ui.disabled = JSONParser::ParseJSONColor(c["disabled"], config.ui.disabled);
        }
    }

    if (configurationJSON.contains("conditions"))
    {
        const json& c = configurationJSON["conditions"];
        config.conditions.interactRadius = c.value("interactRadius", config.conditions.interactRadius);
        config.conditions.facingRadius = c.value("facingRadius", config.conditions.facingRadius);
    }

    if (configurationJSON.contains("spatialBuckets"))
    {
        const json& sb = configurationJSON["spatialBuckets"];
        config.spatialBuckets.tileOnlyRadius = sb.value("tileOnlyRadius", config.spatialBuckets.tileOnlyRadius);
        config.spatialBuckets.nearRadius = sb.value("nearRadius", config.spatialBuckets.nearRadius);
        config.spatialBuckets.mediumRadius = sb.value("mediumRadius", config.spatialBuckets.mediumRadius);
        config.spatialBuckets.farRadius = sb.value("farRadius", config.spatialBuckets.farRadius);
    }

    if (configurationJSON.contains("paths"))
    {
        ModManager& modManager = ModManager::GetInstance();
        const json& p = configurationJSON["paths"];

        std::function<fs::path(const char*, const fs::path&)> getPath = [&](const char* key, const fs::path& defaultValue) -> fs::path {
            if (p.contains(key) && p[key].is_string())
            {
                std::string raw = p.at(key).get<std::string>();
                return modManager.ResolvePath(IOMiscs::MakeCrossPlatformPath(raw));
            }
            return defaultValue;
        };

        config.paths.startingMap = p.value("startingMap", config.paths.startingMap);

        config.paths.mapDirectory = getPath("mapDirectory", config.paths.mapDirectory);
        config.paths.uiFilePath = getPath("uiFilePath", config.paths.uiFilePath);
        config.paths.uiDirectory = getPath("uiDirectory", config.paths.uiDirectory);
        config.paths.saveDirectory = getPath("saveDirectory", config.paths.saveDirectory);
        config.paths.scoreFilePath = getPath("scoreFilePath", config.paths.scoreFilePath);
        config.paths.fontFilePath = getPath("fontFilePath", config.paths.fontFilePath);
        config.paths.fontDirectoryPath = getPath("fontDirectory", config.paths.fontDirectoryPath);
        config.paths.spriteSheetFilePath = getPath("spriteSheetFilePath", config.paths.spriteSheetFilePath);
        config.paths.tileFilePath = getPath("tileFilePath", config.paths.tileFilePath);
        config.paths.presetDirectory = getPath("presetDirectory", config.paths.presetDirectory);
        config.paths.flagFilePath = getPath("flagFilePath", config.paths.flagFilePath);
        config.paths.imagesDirectory = getPath("imagesDirectory", config.paths.imagesDirectory);
        config.paths.audioFilePath = getPath("audioFilePath", config.paths.audioFilePath);
        config.paths.audioDirectory = getPath("audioDirectory", config.paths.audioDirectory);
        config.paths.settingsFilePath = getPath("settingsFilePath", config.paths.settingsFilePath);
        config.paths.cutsceneDirectory = getPath("cutsceneDirectory", config.paths.cutsceneDirectory);
        config.paths.dialogueDirectory = getPath("dialogueDirectory", config.paths.dialogueDirectory);
        config.paths.systemValuesFilePath = getPath("systemValuesFilePath", config.paths.systemValuesFilePath);
    }
}
