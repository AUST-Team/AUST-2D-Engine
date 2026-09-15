#include <SDL3/SDL.h>
#include <fstream>
#include <nlohmann/json.hpp>

#include "Assets.h"

#include "Miscs.h"
#include "MemoryTracker.h"
#include "SDLMiscs.h"

namespace fs = std::filesystem;

SDL_Texture* Assets::spriteSheet = nullptr;

bool Assets::Init(SDL_Renderer* renderer, const fs::path& spriteSheetPath) 
{
    spriteSheet = SDLMiscs::LoadTexture(spriteSheetPath, renderer);

    if (!spriteSheet)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Assets.Init: LoadTexture failed: %s", SDL_GetError());
        return false;
    }

    return true;
}

void Assets::Cleanup() 
{
    if (spriteSheet)
    {
        ENG_SDL_DestroyTexture(spriteSheet);
    }
}

SDL_Texture* Assets::GetSpriteSheet() { return spriteSheet; }