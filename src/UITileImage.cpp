#include <SDL3/SDL.h>

#include "UITileImage.h"

#include "SDL_FRectOperators.h"
#include "TileInstance.h"
#include "TileRegistry.h"
#include "Assets.h"
#include "MemoryTracker.h"

UITileImage::UITileImage(const std::string id, const SDL_FRect& bounds, const SDL_Color& backgroundColor, const SDL_Color& borderColor, int tileIndex, uint8_t transparency) :
    UIElement(id, bounds, backgroundColor, borderColor),
    tileIndex(tileIndex),
    transparency(transparency) { InvalidateCache(); }

void UITileImage::Resize(float widthScale, float heightScale) { UIElement::Resize(widthScale, heightScale); }

void UITileImage::SetTileIndex(int newIndex)
{
    if (tileIndex != newIndex)
    {
        tileIndex = newIndex;
        InvalidateCache();
    }
}

int UITileImage::GetTileIndex() const { return tileIndex; }

void UITileImage::SetBounds(const SDL_FRect& newBounds)
{
    if (bounds != newBounds)
    {
        UIElement::SetBounds(newBounds);
        InvalidateCache();
    }
}

void UITileImage::SetTransparency(uint8_t newTransparency)
{
    if (transparency != newTransparency)
    {
        transparency = newTransparency;
        InvalidateCache();
    }
}

uint8_t UITileImage::GetTransparency() const { return transparency; }

void UITileImage::HandleEvent(const SDL_Event& e) { UIElement::HandleEvent(e); }

void UITileImage::Update(double deltaTime) { UIElement::Update(deltaTime); }

void UITileImage::RebuildCache(SDL_Renderer* renderer) const
{
    if (tileIndex == ConstantConfiguration::invalidTileIndex)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITileImage.RebuildCache: Tile index is invalid.");
        return;
    }

    const TileInstance* tile = TileRegistry::GetTileTypeByIndex(tileIndex);
    if (!tile || tile->renderParts.empty())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITileImage.RebuildCache: Tile index returned null tile type or tile type has no render parts.");
        return;
    }

    const TileRenderPart& part = tile->renderParts[0];

    SDL_Texture* sheet = Assets::GetSpriteSheet();
    if (!sheet)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITileImage.RebuildCache: Sprite sheet is null.");
        return;
    }

    // Create a new 'empty' texture.
    cachedTexture = ENG_SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        static_cast<int>(bounds.w),
        static_cast<int>(bounds.h)
    );

    if (!cachedTexture)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITileImage.RebuildCache: Failed CreateTexture: %s", SDL_GetError());
        return;
    }

    if (!SDL_SetTextureBlendMode(cachedTexture, SDL_BLENDMODE_BLEND))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITileImage.RebuildCache: Failed SetTextureBlendMode: %s", SDL_GetError());
        ENG_SDL_DestroyTexture(cachedTexture);
        cachedTexture = nullptr;
        return;
    }

    // Get old render target.
    SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);

    // Set render target.
    if (!SDL_SetRenderTarget(renderer, cachedTexture))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITileImage.RebuildCache: Failed SetRenderTarget: %s", SDL_GetError());
        ENG_SDL_DestroyTexture(cachedTexture);
        cachedTexture = nullptr;
        return;
    }

    // Clear the renderer.
    if (!SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITileImage.RebuildCache: Failed SetRenderDrawColor: %s", SDL_GetError());
        ENG_SDL_DestroyTexture(cachedTexture);
        cachedTexture = nullptr;
        return;
    }

    if (!SDL_RenderClear(renderer))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITileImage.RebuildCache: Failed RenderClear: %s", SDL_GetError());
        ENG_SDL_DestroyTexture(cachedTexture);
        cachedTexture = nullptr;
        return;
    }

    SDL_FRect dst {
        0.0f, 
        0.0f,
        bounds.w,
        bounds.h
    };

    // Render texture.
    if (!SDL_RenderTexture(renderer, sheet, &part.srcRect, &dst))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITileImage.RebuildCache: Failed RenderTexture: %s", SDL_GetError());
        ENG_SDL_DestroyTexture(cachedTexture);
        cachedTexture = nullptr;
        return;
    }

    // Set back old target.
    if (!SDL_SetRenderTarget(renderer, oldTarget))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITileImage.RebuildCache: Failed SetRenderTarget: %s", SDL_GetError());
        ENG_SDL_DestroyTexture(cachedTexture);
        cachedTexture = nullptr;
        return;
    }

    cachedDimensions = { bounds.w, bounds.h };

    if (transparency < 255)
    {
        if (!SDL_SetTextureAlphaMod(cachedTexture, transparency))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITileImage.RebuildCache: Failed SetTextureBlendMode: %s", SDL_GetError());
            ENG_SDL_DestroyTexture(cachedTexture);
            cachedTexture = nullptr;
            return;
        }
    }
}

void UITileImage::OnRender(SDL_Renderer* renderer) const
{
    EnsureCache(renderer);

    if (!cachedTexture)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITileImage.OnRender: CachedTexture null: %s", SDL_GetError());
        return;
    }

    if (!SDL_RenderTexture(renderer, cachedTexture, nullptr, &bounds))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITileImage.OnRender: Failed RenderTexture: %s", SDL_GetError());
        return;
    }

}

