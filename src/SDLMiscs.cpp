#include <SDL3/SDL_log.h>

#include "SDLMiscs.h"

#include "MemoryTracker.h"

SDL_Texture* SDLMiscs::LoadTexture(const std::filesystem::path& filePath, SDL_Renderer* renderer)
{
    SDL_Surface* surface = ENG_IMG_Load(filePath.string().c_str());

    if (!surface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "LoadTexture: IMG_Load failed for %s: %s", filePath.string().c_str(), SDL_GetError());
        return nullptr;
    }

    SDL_Texture* texture = ENG_SDL_CreateTextureFromSurface(renderer, surface);
    ENG_SDL_DestroySurface(surface);

    if (!texture)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "LoadTexture: CreateTextureFromSurface failed for %s: %s", filePath.string().c_str(), SDL_GetError());
        return nullptr;
    }

    // Sets the scaling mode to nearest neighbor, essential for pixel art.
    if (!SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "LoadTexture: SetTextureScaleMode failed for %s: %s", filePath.string().c_str(), SDL_GetError());
        ENG_SDL_DestroyTexture(texture);
        return nullptr;
    }

    if (!SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "LoadTexture: SetTextureBlendMode failed for %s: %s", filePath.string().c_str(), SDL_GetError());
        ENG_SDL_DestroyTexture(texture);
        return nullptr;
    }

    return texture;
}

SDL_Surface* SDLMiscs::LoadSurface(const std::filesystem::path& filePath)
{
    SDL_Surface* surface = ENG_IMG_Load(filePath.string().c_str());

    if (!surface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "LoadSurface: IMG_Load failed for %s: %s", filePath.string().c_str(), SDL_GetError());
        return nullptr;
    }

    if (!SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "LoadSurface: SetSurfaceBlendMode failed for %s: %s", filePath.string().c_str(), SDL_GetError());
        ENG_SDL_DestroySurface(surface);
        return nullptr;
    }

    return surface;
}
