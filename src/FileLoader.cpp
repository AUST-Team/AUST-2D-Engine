#include <SDL3/SDL_log.h>
#include <fstream>

#include "FileLoader.h"
#include "JSONParser.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

std::optional<json> FileLoader::LoadJSON(const fs::path& filePath, JSONParseFlags loadingFlags)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "FileLoader.LoadJSON: Failed to open JSON file: %s", filePath.string().c_str());
        return std::nullopt;
    }

    try
    {
        json j;
        file >> j;

        if (HasFlag(loadingFlags, JSONParseFlags::ParseDefines))
        {
            JSONParser::ParseJSONDefines(j, loadingFlags);
        }

        return j;
    }
    catch (const json::parse_error& e)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "FileLoader.LoadJSON: Failed to parse JSON file: %s, error: %s", filePath.string().c_str(), e.what());
        return std::nullopt;
    }
    catch (const std::exception& e)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "FileLoader.LoadJSON: Error at parsing: %s, error: %s", filePath.string().c_str(), e.what());
        return std::nullopt;
    }
}