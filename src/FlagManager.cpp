#include <SDL3/SDL_log.h>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "FlagManager.h"

#include "ConstantConfiguration.h"
#include "FileLoader.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

bool FlagManager::LoadFlagsFromFile(const fs::path& filePath)
{
    /*
        What this function expects:

        A 'flags.json' file.
    */

    std::optional<json> jsonOptional = FileLoader::LoadJSON(filePath);

    if (!jsonOptional)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FlagManager.LoadFlagsFromFile: Failed to load JSON at: %s.", filePath.string().c_str());
        return false;
    }

    return LoadFlagsFromJSON(*jsonOptional);
}

void FlagManager::DefineFlag(const std::string& name, bool initialValue)
{
    HashID id = FNV1aHash::HashString(name);

    if (flagIdToIndex.contains(id))
    {
        if (flagIdToName[id] == name)
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "FlagManager.DefineFlag: Duplicate flag '%s'", name.c_str());
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "FlagManager.DefineFlag: Flag ID hash collision '%s' vs '%s'", name.c_str(), flagIdToName[id].c_str());
        }
        return;
    }

    size_t index = flagIdToIndex.size();
    flagIdToIndex[id] = index;
    flagIdToName[id] = name;

    size_t requiredChunks = (index / ConstantConfiguration::bitsPerChunk) + 1;
    if (flagBits.size() < requiredChunks)
    {
        flagBits.resize(requiredChunks, 0);
    }

    size_t chunk = index / ConstantConfiguration::bitsPerChunk;
    size_t bit = index % ConstantConfiguration::bitsPerChunk;

    if (initialValue)
    {
        flagBits[chunk] |= (1ULL << bit);
    }
}

std::string FlagManager::ExportAsHex() const
{
    std::ostringstream oss;
    for (size_t chunk : flagBits) 
    {
        oss << std::hex << chunk << ",";
    }
    return oss.str();
}

void FlagManager::ImportFromHex(const std::string& hexString)
{
    flagBits.clear();

    std::istringstream iss(hexString);
    std::string token;

    while (std::getline(iss, token, ',')) 
    {
        if (token.empty())
        {
            continue;
        }

        uint64_t chunk = 0;
        std::istringstream hexStream(token);
        hexStream >> std::hex >> chunk;
        flagBits.push_back(chunk);
    }
}

void FlagManager::SetFlag(const std::string& name, bool value)
{
    auto it = flagIdToIndex.find(FNV1aHash::HashString(name));
    if (it == flagIdToIndex.end())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "FlagManager.SetFlag: Unknown flag [%s]", name.c_str());
        return;
    }

    size_t index = it->second;
    size_t chunk = index / ConstantConfiguration::bitsPerChunk;
    size_t bit = index % ConstantConfiguration::bitsPerChunk;

    if (value)
    {
        flagBits[chunk] |= (1ULL << bit);
    }
    else
    {
        flagBits[chunk] &= ~(1ULL << bit);
    }
}

bool FlagManager::GetFlag(const std::string& name) const
{
    auto it = flagIdToIndex.find(FNV1aHash::HashString(name));
    if (it == flagIdToIndex.end()) 
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "FlagManager.GetFlag: Unknown flag [%s]", name.c_str());
        return false;
    }

    size_t index = it->second;
    size_t chunk = index / ConstantConfiguration::bitsPerChunk;
    size_t bit = index % ConstantConfiguration::bitsPerChunk;
    return (flagBits[chunk] >> bit) & 1ULL;
}

bool FlagManager::LoadFlagsFromJSON(const nlohmann::json& flagsJSON)
{
    /*
        What this function expects (examples):

        Note that 'flagsJSON' is the default JSON array.

        [
            { "name": "hasSword",                "initial": false },
            { "name": "openedDoor",              "initial": true  },
            { "name": "defeatedBoss",            "initial": false }
        ]
    */

    if (!flagsJSON.is_array())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FlagManager.LoadFlagsFromFile: Expected array, got non-array.");
        return false;
    }

    for (const json& flag : flagsJSON)
    {
        if (!flag.is_object())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "FlagManager.LoadFlagsFromFile: Flag entry is not object.");
            continue;
        }

        if (!flag.contains("name") || !flag["name"].is_string())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "FlagManager.LoadFlagsFromFile: Flag entry doesn't contain name or is not string.");
            continue;
        }

        const std::string& name = flag.at("name").get_ref<const std::string&>();

        if (!flag.contains("initial") || !flag["initial"].is_boolean())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "FlagManager.LoadFlagsFromFile: Flag entry [%s] doesn't contain initial value. Defaulting to \"false\".", name.c_str());
        }

        bool initial = flag.value("initial", false);

        DefineFlag(name, initial);
    }

    return true;
}
