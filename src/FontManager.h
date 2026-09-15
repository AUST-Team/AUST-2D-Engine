#pragma once

#ifndef FONTMANAGER_H_
#define FONTMANAGER_H_

#include <unordered_map>
#include <string>
#include <nlohmann/json.hpp>
#include <filesystem>

#include "Configuration.h"

struct TTF_Font;

/**
* @brief A class that manages all called for fonts by caching them at runtime.
*/
class FontManager 
{
private:
    std::unordered_map<std::filesystem::path, std::unordered_map<float, TTF_Font*>> fontCacheByPath;    /// Font cache by path and size. OWNING pointers.
	std::unordered_map<std::string, TTF_Font*> fontCacheByAlias;	/// Font cache by alias. Non-owning pointers.
    std::unordered_map<std::string, std::filesystem::path> fontFamilies;        /// Font family map, mapping alias to path. Non-owning pointers.

    /**
    * @brief Default constructor.
    */
    FontManager() = default;

    /**
    * @brief Destructor. Calls Cleanup().
    */
    ~FontManager();

    //No coyping or moving allowed due to singleton pattern.
    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;
    FontManager(FontManager&&) = delete;
    FontManager& operator=(FontManager&&) = delete;

    /**
	* @brief Loads fonts from a JSON object. The JSON should contain font family names and their corresponding file paths.
    * 
	* @param fontJSON The JSON object containing font family names and file paths.
    * 
	* @return true if the fonts were loaded successfully, false otherwise.
    */
	bool LoadFontsFromJSON(const nlohmann::json& fontJSON);

public:

    /**
    * @brief Gets the instance of the singleton.
    * 
    * @return A reference to the instance.
    */
    static FontManager& GetInstance() noexcept
    {
        static FontManager instance;
        return instance;
    }

    /**
	* @brief Loads fonts from a JSON file. The file should contain font family names and their corresponding file paths.
    * 
	* @param filePath The path to the JSON file containing font family names and file paths.
    * 
	* @return true if the fonts were loaded successfully, false otherwise.
    */
	bool LoadFontsFromFile(const std::filesystem::path& filePath = Configuration::Get().paths.fontFilePath);

    /**
	* @brief Registers a font family with a given name and file path.
    * 
	* @param familyName The name of the font family.
	* @param filePath The file path to the font file.
    */
    void RegisterFontFamily(const std::string& familyName, const std::filesystem::path& filePath);

    /**
	* @brief Gets the font from a registered family name and size.
    * 
	* @param familyName The name of the font family.
	* @param size The size of the font.
    * 
	* @return A TTF_Font pointer to the font or nullptr if the family name is not registered.
    */
    TTF_Font* GetFontByFamily(const std::string& familyName, float size);

    /**
    * @brief Gets the font with a certain size by the path. If the font is not cached, it will be loaded and cached.
    * 
    * @param path Path to the font.
    * @param size Size of the font.
    * 
    * @return A TTF_Font pointer to the font or nullptr if error.
    */
    TTF_Font* GetFontByPath(const std::filesystem::path& path, float size);

    /**
    * @brief Cleans up the font cache.
    */
    void Cleanup();

    bool LoadFontsFromFile(const std::string& filePath) = delete;
    bool LoadFontsFromFile(const char* filePath) = delete;
    void RegisterFontFamily(const std::string& familyName, const std::string& filePath) = delete;
    void RegisterFontFamily(const std::string& familyName, const char * filePath) = delete;
    TTF_Font* GetFontByPath(const std::string& path, float size) = delete;
    TTF_Font* GetFontByPath(const char * path, float size) = delete;
};

#endif // FONTMANAGER_H_