#include <algorithm>
#include <nlohmann/json.hpp>
#include <fstream>
#include <SDL3/SDL.h>

#include "ScoreManager.h"
#include "Configuration.h"
#include "FileLoader.h"
#include "JSONParser.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

bool ScoreManager::LoadScoresFromFile(const fs::path& filePath)
{
    scores.clear();

    std::optional<json> jsonOptional = FileLoader::LoadJSON(filePath, JSONParseFlags::None);

    if (!jsonOptional)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ScoreManager.LoadScoresFromFile: Failed to open %s", filePath.string().c_str());
        return false;
    }

    return LoadScoresFromJSON(*jsonOptional);
}

bool ScoreManager::LoadScoresFromJSON(const json& scoresJSON)
{
    /*
        What this function expects (examples):

        Note that 'scoresJSON' is the default JSON array.

        [
            { "name": "PlayerOne", "caught": 2, "time": 145 },
            { "name": "PlayerTwo", "caught": 0, "time": 115 }
        ]
    */

    if (!scoresJSON.is_array())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ScoreManager.LoadScoresFromJSON: Expected array, got non-array.");
        return false;
    }

    for (const json& entry : scoresJSON)
    {
        const ScoreEntry scoreEntry = JSONParser::ParseJSONScoreEntry(entry, ScoreEntry{ .name = "", .timesCaught = -1 });

        if(scoreEntry.name.empty() || scoreEntry.timesCaught < 0)
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ScoreManager.LoadScoresFromJSON: Invalid score entry, skipping.");
            continue;
		}

        scores.emplace_back(scoreEntry);
    }

    return true;
}

bool ScoreManager::AddScore(const std::string& name, int timesCaught, uint64_t timePlayedSeconds, const fs::path& filePath)
{
    for (ScoreEntry& entry : scores)
    {
        if (entry.name == name)
        {
            const int oldCaught = entry.timesCaught;
            const uint64_t oldTime = entry.timePlayed;

            if (timesCaught < oldCaught || (timesCaught == oldCaught && timePlayedSeconds < oldTime))
            {
                entry = ScoreEntry { .name = name, .timesCaught = timesCaught, .timePlayed = timePlayedSeconds };
                return SaveToFile(filePath);
            }

            return true;
        }
    }

    scores.emplace_back(name, timesCaught, timePlayedSeconds);
    return SaveToFile(filePath);
}

bool ScoreManager::AddScore(const ScoreEntry& entry, const fs::path& filePath)
{
    return AddScore(entry.name, entry.timesCaught, entry.timePlayed, filePath);
}

std::vector<ScoreEntry> ScoreManager::GetTop15Scores() const
{
    std::vector<ScoreEntry> top = scores;
    std::sort(top.begin(), top.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
        if (a.timesCaught != b.timesCaught)
        {
            return a.timesCaught < b.timesCaught;
        }
        return a.timePlayed < b.timePlayed;
    });

    if (top.size() > 15)
    {
        top.resize(15);
    }

    return top;
}

bool ScoreManager::ClearScores(const fs::path& filePath)
{
    scores.clear();
    return SaveToFile(filePath);
}

bool ScoreManager::SaveToFile(const fs::path& filePath)
{
    json scoresArray = json::array();

    for (const ScoreEntry& entry : scores)
    {
        scoresArray.push_back({
            { "name", entry.name },
            { "caught", entry.timesCaught },
            { "time", entry.timePlayed }
        });
    }

    json j;
    j["scores"] = scoresArray;

    std::ofstream file(filePath, std::ios::trunc);
    if (!file.is_open())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ScoreManager.SaveToFile: Failed to write to file: %s", filePath.string().c_str());
        return false;
    }

    try
    {
        file << j.dump(4);
    }
    catch (const std::exception& e)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ScoreManager.SaveToFile: Error at writing in file: %s, error: %s", filePath.string().c_str(), e.what());
        return false;
    }

    return true;
}
