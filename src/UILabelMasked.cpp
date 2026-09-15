#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cmath>

#include "UILabelMasked.h"

#include "MemoryTracker.h"

UILabelMasked::UILabelMasked(const std::string id, const SDL_FRect& bounds, const SDL_Color& backgroundColor, const SDL_Color& borderColor, const std::string& text, TTF_Font* textFont, const SDL_Color& textColor, const UITextAlignment& textAlignment, const std::string& fontFamily, SDL_Texture* backgroundTexture) :
    UILabel(id, bounds, backgroundColor, borderColor, text, textFont, textColor, textAlignment, fontFamily),
    backgroundTexture(backgroundTexture) {}

UILabelMasked::~UILabelMasked()
{
    if (backgroundTexture)
    {
        ENG_SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
}

UILabelMasked::UILabelMasked(UILabelMasked&& other) noexcept :
    UILabel(std::move(other)), backgroundTexture(other.backgroundTexture)
{
    other.backgroundTexture = nullptr;
}

void UILabelMasked::SetBackgroundTexture(SDL_Texture* newTexture) 
{ 
    if (backgroundTexture != newTexture) 
    {
        if (backgroundTexture)
        {
            ENG_SDL_DestroyTexture(backgroundTexture);
        }
        backgroundTexture = newTexture;
        InvalidateCache();
    }
}

UILabelMasked& UILabelMasked::operator=(UILabelMasked&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    *((UILabel*)this) = std::move(other);

    if (backgroundTexture)
    {
        ENG_SDL_DestroyTexture(backgroundTexture);
    }

    backgroundTexture = other.backgroundTexture;

    other.backgroundTexture = nullptr;

    return *this;
}

// Oh boy.
void UILabelMasked::RebuildCache(SDL_Renderer* renderer) const
{
    if (!needsUpdate)
    {
        // No update needed.
        return;
    }

    if (text.empty())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UILabelMasked.RebuildCache: Text is empty.");
        return;
    }

    if (!textFont)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UILabelMasked.RebuildCache: Font is null for %s.", text.c_str());
        return;
    }

    if (!backgroundTexture)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UILabelMasked.RebuildCache: Background texture is null for %s.", text.c_str());
        return;
    }

    if (cachedTexture)
    {
        ENG_SDL_DestroyTexture(cachedTexture);
        cachedTexture = nullptr;
    }

    // Get the text surface.
    SDL_Color color { 255, 255, 255, 255 };
    SDL_Surface* textSurface = ENG_TTF_RenderText_Blended(textFont, text.c_str(), 0, color);
    if (!textSurface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UILabelMasked.RebuildCache: Failed RenderTextBlended for %s: %s", text.c_str(), SDL_GetError());
        return;
    }

    cachedDimensions = { static_cast<float>(textSurface->w), static_cast<float>(textSurface->h) };

    // Get the text texture.
    SDL_Texture* textMask = ENG_SDL_CreateTextureFromSurface(renderer, textSurface);
    ENG_SDL_DestroySurface(textSurface);
    if (!textMask)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UILabelMasked.RebuildCache: Failed CreateTextureFromSurface for %s: %s", text.c_str(), SDL_GetError());
        return;
    }

    // Set the blending mode on text texture.
    // src = source = testMask.
    // dst = destination = the texture this is placed ontop of.
    const bool success = SDL_SetTextureBlendMode(textMask, SDL_ComposeCustomBlendMode(
        SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD,  // dstRGB = dstRGB
        SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ZERO, SDL_BLENDOPERATION_ADD   // dstA = srcA
    ));

    if (!success)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UILabelMasked.RebuildCache: Failed SetTextureBlendMode for %s: %s", text.c_str(), SDL_GetError());
        ENG_SDL_DestroyTexture(textMask);
        return;
    }

    // Create a mini-buffer (mini-canvas) we can draw on.
    // This is very important as to not drawn on the main window.
    // If we did, everything would be blended using textMask's blending mode (bad).
    SDL_Texture* rt = ENG_SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET,
        static_cast<int> (cachedDimensions.w),
        static_cast<int> (cachedDimensions.h)
    );

    if (!rt)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UILabelMasked.RebuildCache: Failed CreateTexture for %s: %s", text.c_str(), SDL_GetError());
        ENG_SDL_DestroyTexture(textMask);
        return;
    }

    // Set the blend mode on the mini-buffer to 'BLEND' (so transparency is taken into account).
    if (!SDL_SetTextureBlendMode(rt, SDL_BLENDMODE_BLEND))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UILabelMasked.RebuildCache: Failed SetTextureBlendMode for %s: %s", text.c_str(), SDL_GetError());
        ENG_SDL_DestroyTexture(rt);
        ENG_SDL_DestroyTexture(textMask);
        return;
    }

    // Get the old render target.
    SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);

    // Set the renderer to only draw on the mini-buffer.
    if (!SDL_SetRenderTarget(renderer, rt))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UILabelMasked.RebuildCache: Failed SetRenderTarget for %s: %s", text.c_str(), SDL_GetError());
        ENG_SDL_DestroyTexture(rt);
        ENG_SDL_DestroyTexture(textMask);
        return;
    }

    if (!SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UILabelMasked.RebuildCache: Failed SetRenderDrawColor for %s: %s", text.c_str(), SDL_GetError());
        ENG_SDL_DestroyTexture(rt);
        ENG_SDL_DestroyTexture(textMask);
        return;
    }

    if (!SDL_RenderClear(renderer))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UILabelMasked.RebuildCache: Failed RenderClear for %s: %s", text.c_str(), SDL_GetError());
        ENG_SDL_DestroyTexture(rt);
        ENG_SDL_DestroyTexture(textMask);
        return;
    }

    SDL_FRect dst = { 0, 0, cachedDimensions.w, cachedDimensions.h };

    // First we render the background image (destination).
    if (!SDL_RenderTexture(renderer, backgroundTexture, nullptr, &dst))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UILabelMasked.RebuildCache: Failed RenderTexture for %s: %s", text.c_str(), SDL_GetError());
        ENG_SDL_DestroyTexture(rt);
        ENG_SDL_DestroyTexture(textMask);
        return;
    }

    // Then we overlay the text over it (source).
    if (!SDL_RenderTexture(renderer, textMask, nullptr, &dst))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UILabelMasked.RebuildCache: Failed RenderTexture for %s: %s", text.c_str(), SDL_GetError());
        ENG_SDL_DestroyTexture(rt);
        ENG_SDL_DestroyTexture(textMask);
        return;
    }

    // We change the renderer to the be whole window.
    if (!SDL_SetRenderTarget(renderer, oldTarget))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UILabelMasked.RebuildCache: Failed SetRenderTarget for %s: %s", text.c_str(), SDL_GetError());
        ENG_SDL_DestroyTexture(rt);
        ENG_SDL_DestroyTexture(textMask);
        return;
    }

    cachedTexture = rt;

    rt = nullptr;
    ENG_SDL_DestroyTexture(textMask);
    needsUpdate = false;
}