#include "UITextElement.h"

#include "SDL_ColorOperators.h"

UITextElement::UITextElement(const std::string& text, TTF_Font* textFont, const SDL_Color& textColor, const UITextAlignment& textAlignment, const std::string& fontFamily) :
    UITextStyleElement(textFont, textColor, textAlignment, fontFamily),
    text(text) {}

void UITextElement::SetText(const std::string& newText)
{
    if (text != newText)
    {
        text = newText;
    }
}

std::string UITextElement::GetText() const { return text; }

void UITextElement::SetTextEffectKey(const std::string& newEffectKey) { textEffectKey = newEffectKey; }

std::string UITextElement::GetTextEffectKey() const { return textEffectKey; }
