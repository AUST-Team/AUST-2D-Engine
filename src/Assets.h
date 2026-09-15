#pragma once

#ifndef ASSETS_H_
#define ASSETS_H_

#include <unordered_map>
#include <string>
#include <filesystem>

#include "TileInstance.h"
#include "Configuration.h"

/**
* @brief Static class. Hold the assets of the game; the sprite sheet and a map of <TileInstance, SDL_FRect*>
* 
* Shouldn't be instanced.
*/
class Assets 
{
private:
    static SDL_Texture* spriteSheet;    /// Pointer to a SDL_Texture containing the sprite sheet.

    // No instancing due to static class.
    Assets() = delete;
    ~Assets() = delete;

    // No copying or moving allowed due to static class.
    Assets(const Assets&) = delete;
    Assets& operator=(const Assets&) = delete;
    Assets(Assets&&) = delete;
    Assets& operator=(Assets&&) = delete;

public:
    
    /**
    * @brief Initialises the assets.
    * 
    * @param renderer Pointer to an SDL renderer.
    * @param spriteSheetPath Path to the sprite sheet.
    * 
    * @return Returns true if succesful, false if not.
    */
    static bool Init(SDL_Renderer* renderer, const std::filesystem::path& spriteSheetPath = Configuration::Get().paths.spriteSheetFilePath);


    /**
    * @brief Cleans up the assets of the game.
    */
    static void Cleanup();

    /**
    * @brief Returns the sprite sheet.
    * 
    * @return SDL_Texture pointer.
    */
    static SDL_Texture* GetSpriteSheet();

    static bool Init(SDL_Renderer* renderer, const std::string& spriteSheetPath) = delete;
    static bool Init(SDL_Renderer* renderer, const char* spriteSheetPath) = delete;
};

#endif // ASSETS_H_