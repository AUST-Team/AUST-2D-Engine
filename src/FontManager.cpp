#include <SDL3_ttf/SDL_ttf.h>

#include "FontManager.h"
#include "FileLoader.h"
#include "MemoryTracker.h"
#include "IOMiscs.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

FontManager::~FontManager() { Cleanup(); }

bool FontManager::LoadFontsFromFile(const fs::path& filePath)
{
    const std::optional<json> jsonOptional = FileLoader::LoadJSON(filePath);

    if (!jsonOptional)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FontManager.LoadFontsFromFile: Failed to open file %s", filePath.string().c_str());
        return false;
    }

    return LoadFontsFromJSON(*jsonOptional);
}

void FontManager::RegisterFontFamily(const std::string& familyName, const fs::path& filePath)
{
    fontFamilies[familyName] = filePath;
}

TTF_Font* FontManager::GetFontByFamily(const std::string& familyName, float size)
{
    auto it = fontFamilies.find(familyName);
    if (it == fontFamilies.end())
    {
        return nullptr;
    }
    return GetFontByPath(it->second, size);
}

TTF_Font* FontManager::GetFontByPath(const fs::path& path, float size)
{
    // Check if the path is already in the cache.
    auto pathIt = fontCacheByPath.find(path);
    if (pathIt != fontCacheByPath.end())
    {
        // Get the size - font map.
        const std::unordered_map<float, TTF_Font*>& sizeMap = pathIt->second;

        // Check if the size is already in the cache.
        auto sizeIt = sizeMap.find(size);

        // If the size is already in the cache, return it.
        if (sizeIt != sizeMap.end())
        {
            return sizeIt->second;
        }
    }

    // Not cached yet.
    TTF_Font* font = ENG_TTF_OpenFont(path.string().c_str(), size);
    if (!font)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FontManager.GetFontByPath: Failed to load font %s size %f: %s", path.string().c_str(), size, SDL_GetError());
        return nullptr;
    }

    fontCacheByPath[path][size] = font;
    return font;
}

void FontManager::Cleanup()
{
    for (auto& [path, sizeMap] : fontCacheByPath) 
    {
        for (auto& [size, font] : sizeMap) 
        {
            if (font)
            {
                ENG_TTF_CloseFont(font);
            }
        }
    }
    fontCacheByPath.clear();
	fontCacheByAlias.clear();
}

bool FontManager::LoadFontsFromJSON(const json& fontJSON)
{
    /*
        What this function expects:

        Note that fontJSON is the default JSON object
        {
            "Title": "arial.ttf",
            "Button": "times_new_roman.ttf"
		}
    */

    if (!fontJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FontManager.LoadFontsFromJSON: Expected JSON object for font families.");
		return false;
    }

    for (auto& [familyName, pathJSON] : fontJSON.items())
    {
        if(!pathJSON.is_string())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FontManager.LoadFontsFromJSON: Expected string for font path of family '%s'.", familyName.c_str());
            continue;
		}

		const std::string& path = pathJSON.get_ref<const std::string&>();
        RegisterFontFamily(familyName, Configuration::Get().paths.fontDirectoryPath / IOMiscs::MakeCrossPlatformPath(path));
    }

    return true;
}


