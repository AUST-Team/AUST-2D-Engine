#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <filesystem>

#include "AppRenderSystem.h"

#include "MemoryTracker.h"

namespace fs = std::filesystem;

AppRenderSystem::~AppRenderSystem() { Cleanup(); }

bool AppRenderSystem::Init(const std::string& title, int w, int h)
{
    Cleanup();

    dimensions = { w, h };

    // | SDL_WINDOW_HIDDEN
    window = ENG_SDL_CreateWindow(title.c_str(), w, h, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    if (!window) 
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "AppRenderSystem.Init: CreateWindow failed: %s", SDL_GetError());
        return false;
    }

    const GameConfiguration& config = Configuration::Get();
    const fs::path iconPath = config.paths.imagesDirectory / config.window.iconFileName;
    SDL_Surface* iconSurface = ENG_IMG_Load(iconPath.string().c_str());

    if (iconSurface)
    {
        if (!SDL_SetWindowIcon(window, iconSurface))
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "AppRenderSystem.Init: SetWindowIcon failed: %s", SDL_GetError());
        }
        ENG_SDL_DestroySurface(iconSurface);
    }

    renderer = ENG_SDL_CreateRenderer(window, nullptr);
    if (!renderer) 
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "AppRenderSystem.Init: CreateRenderer failed: %s", SDL_GetError());
        return false;
    }

    if ( !SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) )
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AppRenderSystem.Init: SetRenderDrawBlendMode failed: %s", SDL_GetError());
        return false;
    }

    return true;
}

void AppRenderSystem::Cleanup() 
{
    if (renderer) 
    {
        ENG_SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) 
    {
        ENG_SDL_DestroyWindow(window);
        window = nullptr;
    }
}

void AppRenderSystem::UpdateWindowSize()
{
    if (!SDL_GetWindowSizeInPixels(window, &dimensions.w, &dimensions.h))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AppRenderSystem.UpdateWindowSize(): GetWindowSizeInPixels failed: %s", SDL_GetError());
    }
}

void AppRenderSystem::ProcessPendingWindowChanges()
{
    if (pendingFullscreenFlag)
    {
        pendingFullscreenFlag = false;
        if (!SDL_SetWindowFullscreen(window, fullscreenStatusFlag ? SDL_WINDOW_FULLSCREEN : 0))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AppRenderSystem.ProcessPendingWindowChanges: Failed SetWindowFullscreen: %s", SDL_GetError());
        }
        // Do not change the window size here, as this will trigger a SDL_EVENT_WINDOW_RESIZED, and some components might need to get the old size too.
    }

    if (pendingSizeChangeFlag)
    {
        pendingSizeChangeFlag = false;
        if (!SDL_SetWindowSize(window, pendingDimensions.w, pendingDimensions.h))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AppRenderSystem.ProcessPendingWindowChanges: Failed SDL_SetWindowSize: %s", SDL_GetError());
        }
    }
}

SDL_Renderer* AppRenderSystem::GetRenderer() const { return renderer; }

SDL_Window* AppRenderSystem::GetWindow() const { return window; }

int AppRenderSystem::GetWidth() const { return dimensions.w; }

int AppRenderSystem::GetHeight() const { return dimensions.h; }

Size AppRenderSystem::GetSafeWindowSize() const
{
    Size bounds = { 0, 0 };

    if ( !SDL_GetWindowSizeInPixels(window, &bounds.w, &bounds.h))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AppRenderSystem.GetSafeWindowSize(): GetWindowSizeInPixels failed: %s", SDL_GetError());
    }

    SDL_WindowFlags flags = SDL_GetWindowFlags(window);

    if (flags & SDL_WINDOW_MAXIMIZED)
    {
        int displayIndex = SDL_GetDisplayForWindow(window);
        if (displayIndex <= 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AppRenderSystem.GetSafeWindowSize: GetDisplayForWindow failed: %s", SDL_GetError());
            return bounds;
        }

        SDL_Rect usable;

        if (!SDL_GetDisplayUsableBounds(displayIndex, &usable))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AppRenderSystem.GetSafeWindowSize: GetDisplayUsableBounds failed: %s", SDL_GetError());
        }

        bounds = { usable.w, usable.h };
    }

    return bounds;
}

void AppRenderSystem::ScheduleWindowSizeChange(Size newDimensions)
{
    pendingDimensions = newDimensions;
    pendingSizeChangeFlag = true;
}

void AppRenderSystem::ScheduleFullScreen(bool fullscreen)
{
    pendingFullscreenFlag = true;
    fullscreenStatusFlag = fullscreen;
}

bool AppRenderSystem::IsFullscreen() const
{
    SDL_WindowFlags flags = SDL_GetWindowFlags(window);

    return (flags & SDL_WINDOW_FULLSCREEN);
}

void AppRenderSystem::ShowWindow(bool show)
{
    if (show)
    {
        if (!SDL_ShowWindow(window))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AppRenderSystem.ShowWindow: Failed ShowWindow: %s", SDL_GetError());
        }
    }
    else
    {
        if (!SDL_HideWindow(window))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AppRenderSystem.ShowWindow: Failed HideWindow: %s", SDL_GetError());
        }
    }
}
