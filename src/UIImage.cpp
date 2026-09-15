#include <SDL3/SDL.h>
#include <utility>
#include <algorithm>

#include "UIImage.h"

#include "SDL_ColorOperators.h"
#include "SDL_FRectOperators.h"
#include "MemoryTracker.h"

UIImage::UIImage(const std::string id, const SDL_FRect& bounds, const SDL_Color& backgroundColor, const SDL_Color& borderColor, SDL_Surface* surface, uint8_t transparency) :
    UIElement(id, bounds, backgroundColor, borderColor),
    originalSurface(surface),
    transparency(transparency) { InvalidateCache(); }

UIImage::UIImage(UIImage&& other) noexcept :
	UIElement(std::move(other)),
	UICachedTextureElement(std::move(other)),
	originalSurface(std::exchange(other.originalSurface, nullptr)),
    transparency(other.transparency) {}

UIImage::~UIImage()
{
	if (originalSurface)
	{
        ENG_SDL_DestroySurface(originalSurface);
		originalSurface = nullptr;
	}
}

void UIImage::SetImageSurface(SDL_Surface* newSurface)
{
    if (originalSurface != newSurface)
    {
        if (originalSurface)
        {
            ENG_SDL_DestroySurface(originalSurface);
        }

        originalSurface = newSurface;
        InvalidateCache();
    }
}

SDL_Surface* UIImage::GetImageSurface() const { return originalSurface; }

void UIImage::SetBounds(const SDL_FRect& newBounds)
{
	if (bounds != newBounds)
	{
		UIElement::SetBounds(newBounds);
		InvalidateCache();
	}
}

void UIImage::SetTransparency(uint8_t newTransparency)
{
    if (transparency != newTransparency)
    {
        transparency = newTransparency;
        InvalidateCache();
    }
}

uint8_t UIImage::GetTransparency() const { return transparency; }

UIImage& UIImage::operator=(UIImage&& other) noexcept
{
	if (this == &other)
	{
        return *this;
	}

    (*(UIElement*)this) = std::move(other);
    (*(UICachedTextureElement*)this) = std::move(other);

    originalSurface = std::exchange(other.originalSurface, nullptr);
    transparency = other.transparency;

	return *this;
}

void UIImage::RebuildCache(SDL_Renderer* renderer) const
{
    if (!originalSurface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIImage.RebuildCache: No original surface to rebuild from");
        return;
    }

    SDL_Surface* resizedSurface = ENG_SDL_CreateSurface(
        static_cast<int>(std::round(bounds.w)), 
        static_cast<int>(std::round(bounds.h)),
        originalSurface->format
    );

    if (!resizedSurface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIImage.RebuildCache: Failed CreateSurface: %s", SDL_GetError());
        return;
    }

    SDL_Rect srcRect { 0, 0, originalSurface->w, originalSurface->h };
    SDL_Rect dstRect { 0, 0, static_cast<int>(std::round(bounds.w)), static_cast<int>(std::round(bounds.h)) };
    if (!SDL_BlitSurfaceScaled(originalSurface, &srcRect, resizedSurface, &dstRect, SDL_SCALEMODE_LINEAR))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIImage.RebuildCache: Failed BlitSurfaceScaled: %s", SDL_GetError());
        ENG_SDL_DestroySurface(resizedSurface);
        return;
    }

    cachedTexture = ENG_SDL_CreateTextureFromSurface(renderer, resizedSurface);

    cachedDimensions = { static_cast<float>(resizedSurface->w), static_cast<float>(resizedSurface->h) };

    ENG_SDL_DestroySurface(resizedSurface);

    if (!cachedTexture)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIImage.RebuildCache: Failed to create texture: %s", SDL_GetError());
        return;
    }

    if (transparency < 255)
    {
        if (!SDL_SetTextureBlendMode(cachedTexture, SDL_BLENDMODE_BLEND))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIImage.RebuildCache: Failed SetTextureBlendMode: %s", SDL_GetError());
            ENG_SDL_DestroyTexture(cachedTexture);
            cachedTexture = nullptr;
            return;
        }

        if (!SDL_SetTextureAlphaMod(cachedTexture, transparency))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIImage.RebuildCache: Failed SetTextureBlendMode: %s", SDL_GetError());
            ENG_SDL_DestroyTexture(cachedTexture);
            cachedTexture = nullptr;
            return;
        }
    }
}

void UIImage::OnRender(SDL_Renderer* renderer) const
{
    EnsureCache(renderer);

    if (!cachedTexture)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIImage.OnRender: CachedTexture null: %s", SDL_GetError());
        return;
    }

    if (!SDL_RenderTexture(renderer, cachedTexture, nullptr, &bounds))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIImage.OnRender: Failed RenderTexture: %s", SDL_GetError());
        return;
    }

}
