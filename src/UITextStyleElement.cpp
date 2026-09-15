#include <SDL3_ttf/SDL_ttf.h>

#include "UITextStyleElement.h"

#include "SDL_ColorOperators.h"

UITextStyleElement::UITextStyleElement(TTF_Font* textFont, const SDL_Color& textColor, const UITextAlignment& textAlignment, const std::string& fontFamily) :
    textFont(textFont),
    textColor(textColor),
    textAlignment(textAlignment),
    originalFontSize(TTF_GetFontSize(textFont)),
    currentFontHeight(TTF_GetFontHeight(textFont)),
    originalFontHeight(currentFontHeight),
    fontFamily(fontFamily) {}


void UITextStyleElement::SetTextColor(const SDL_Color& newColor)
{
    if (textColor != newColor)
    {
        textColor = newColor;
    }
}

SDL_Color UITextStyleElement::GetTextColor() const { return textColor; }

float UITextStyleElement::GetOriginalFontSize() const { return originalFontSize; }

size_t UITextStyleElement::GetCurrentFontHeight() const { return currentFontHeight; }

size_t UITextStyleElement::GetOriginalFontHeight() const { return originalFontHeight; }

void UITextStyleElement::SetTextFont(TTF_Font* newFont)
{
    if (newFont && textFont != newFont)
    {
        textFont = newFont;
        currentFontHeight = TTF_GetFontHeight(textFont);
    }
}

void UITextStyleElement::OverrideTextFont(TTF_Font* newFont, const std::string& newFontFamily)
{
    if (newFont && textFont != newFont && fontFamily != newFontFamily)
    {
		fontFamily = newFontFamily;
        textFont = newFont;
		originalFontSize = TTF_GetFontSize(newFont);
        currentFontHeight = TTF_GetFontHeight(textFont);
        originalFontHeight = currentFontHeight;
    }
}

TTF_Font* UITextStyleElement::GetTextFont() const { return textFont; }

std::string UITextStyleElement::GetFontFamily() const { return fontFamily; }

void UITextStyleElement::SetTextAlignment(const UITextAlignment& newAlignment) { textAlignment = newAlignment; }

UITextAlignment UITextStyleElement::GetTextAlignment() const { return textAlignment; }
