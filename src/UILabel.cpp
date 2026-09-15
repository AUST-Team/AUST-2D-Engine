#include <SDL3/SDL.h>
#include <cmath>
#include <SDL3_ttf/SDL_ttf.h>

#include "UILabel.h"

#include "SDL_FRectOperators.h"
#include "SDL_ColorOperators.h"
#include "UITextEffectSystem.h"
#include "FontManager.h"
#include "Unused.h"

UILabel::UILabel(const std::string id, const SDL_FRect& bounds, const SDL_Color& backgroundColor, const SDL_Color& borderColor, const std::string& text, TTF_Font* textFont, const SDL_Color& textColor, const UITextAlignment& textAlignment, const std::string& fontFamily) :
    UIElement(id, bounds, backgroundColor, borderColor),
	UICachedTextElement(text, textFont, textColor, textAlignment, fontFamily) {}


void UILabel::Update(double deltaTime)
{
    UNUSED(deltaTime);
    if (!textEffectKey.empty() && UITextEffectSystem::GetInstance().HasEffect(textEffectKey))
    {
        std::string currentText = UITextEffectSystem::GetInstance().GetText(textEffectKey);
        SetText(currentText.empty() ? " " : currentText);
    }
}

void UILabel::HandleEvent(const SDL_Event& e) { UIElement::HandleEvent(e); }

void UILabel::Resize(float widthScale, float heightScale)
{
	UIElement::Resize(widthScale, heightScale);

	float fontScale = std::min(widthScale, heightScale);
	float newFontSize = originalFontSize * fontScale;

	TTF_Font* newFont = FontManager::GetInstance().GetFontByFamily(fontFamily, newFontSize);

    if (!newFont)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UILabel Resize: Failed to load new font for %s: %s", text.c_str(), SDL_GetError());
        return;
    }

	SetTextFont(newFont);
}

void UILabel::RenderOffset(SDL_Renderer* renderer, const SDL_FPoint& offset) const
{
    if (!visibleFlag)
    {
        return;
    }

    if (text.empty())
    {
        return;
    }

    EnsureCache(renderer);

    if (!cachedTexture)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UILabel.RenderOffset: CachedTexture null for %s: %s", text.c_str(), SDL_GetError());
        return;
    }

    FSize size = GetCachedSize();

    SDL_FPoint alignedPosition = GetAlignedPosition(bounds, size.w, size.h, textAlignment);

    SDL_FRect dst {
        std::round(alignedPosition.x) + offset.x,
        std::round(alignedPosition.y) + offset.y,
        size.w,
        size.h
    };

    if (!SDL_RenderTexture(renderer, cachedTexture, nullptr, &dst))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UILabel.RenderOffset: Failed RenderTexture for %s: %s", text.c_str(), SDL_GetError());
    }
}

void UILabel::SetBounds(const SDL_FRect& newBounds)
{
    if (bounds != newBounds)
    {
        UIElement::SetBounds(newBounds);
        InvalidateCache();
    }
}

void UILabel::OnRender(SDL_Renderer* renderer) const { RenderOffset(renderer, SDL_FPoint { 0.0f, 0.0f }); }

