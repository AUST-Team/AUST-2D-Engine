#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "UICachedTextElement.h"

#include "SDL_ColorOperators.h"
#include "MemoryTracker.h"

UICachedTextElement::UICachedTextElement(const std::string& text, TTF_Font* textFont, const SDL_Color& textColor, const UITextAlignment& textAlignment, const std::string& fontFamily) :
    UITextElement(text, textFont, textColor, textAlignment, fontFamily) { InvalidateCache(); }


void UICachedTextElement::SetTextFont(TTF_Font* newFont)
{ 
    if (textFont != newFont)
    {
        UITextElement::SetTextFont(newFont);
        InvalidateCache();
    }
}

void UICachedTextElement::OverrideTextFont(TTF_Font* newFont, const std::string& newFontFamily)
{
    if (textFont != newFont || fontFamily != newFontFamily)
    {
        UITextElement::OverrideTextFont(newFont, newFontFamily);
        InvalidateCache();
    }
}

void UICachedTextElement::SetText(const std::string& newText)
{
    if (text != newText)
    {
        UITextElement::SetText(newText);
        InvalidateCache();
    }
}

void UICachedTextElement::SetTextColor(const SDL_Color& newColor) 
{ 
    if (textColor != newColor)
    {
        UITextElement::SetTextColor(newColor);
        InvalidateCache();
    }
}

void UICachedTextElement::SetTextAlignment(const UITextAlignment& newAlignment)
{
    if (textAlignment != newAlignment)
    {
        UITextElement::SetTextAlignment(newAlignment);
        InvalidateCache();
    }
}

void UICachedTextElement::RebuildCache(SDL_Renderer* renderer) const
{
    if (text.empty())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UICachedTextElement: Text empty.");
        return;
    }

    if (!textFont)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UICachedTextElement.RebuildCache: Font is null for %s.", text.c_str());
        return;
    }

    SDL_Surface* surface = ENG_TTF_RenderText_Blended_Wrapped(textFont, text.c_str(), 0, textColor, 0);
    if (!surface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UICachedTextElement.RebuildCache: Failed RenderText_Blended for %s: %s", text.c_str(), SDL_GetError());
        return;
    }

    cachedTexture = ENG_SDL_CreateTextureFromSurface(renderer, surface);

    cachedDimensions = { static_cast<float>(surface->w), static_cast<float>(surface->h) };

    ENG_SDL_DestroySurface(surface);
    if (!cachedTexture)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UICachedTextElement.RebuildCache: Failed CreateTextureFromSurface for %s: %s", text.c_str(), SDL_GetError());
        return;
    }
}

