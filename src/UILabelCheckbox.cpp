#include <SDL3/SDL.h>

#include "UILabelCheckbox.h"

UILabelCheckbox::UILabelCheckbox(const std::string id, const SDL_FRect& bounds, const SDL_Color& backgroundColor, const SDL_Color& borderColor, const std::string& text, TTF_Font* textFont, const SDL_Color& textColor, const UITextAlignment& textAlignment, const std::string& fontFamily, const SDL_Color& boxColor, const SDL_Color& checkColor, CheckboxState initialState, float boxSize, float spacing) :
    UILabel(id, bounds, backgroundColor, borderColor, text, textFont, textColor, textAlignment, fontFamily),
    boxColor(boxColor),
    checkColor(checkColor),
    checkedFlag(initialState),
    boxSize(boxSize),
    spacing(spacing) {}

void UILabelCheckbox::HandleEvent(const SDL_Event& e)
{
    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == SDL_BUTTON_LEFT)
    {
        float mouseX = static_cast<float>(e.button.x);
        float mouseY = static_cast<float>(e.button.y);
        if (Contains(mouseX, mouseY))
        {
            checkedFlag = !checkedFlag;
            if (onToggle)
            {
                onToggle(checkedFlag);
            }
        }
    }
}

void UILabelCheckbox::Resize(float widthScale, float heightScale)
{
    UILabel::Resize(widthScale, heightScale);

    float minScale = std::min(widthScale, heightScale);
    boxSize = originalBoxSize * minScale;
}

void UILabelCheckbox::SetOnToggle(const std::function<void(bool)>& callback) { onToggle = callback; }

void UILabelCheckbox::SetCheckBoxColor(const SDL_Color& newColor) { boxColor = newColor; }

SDL_Color UILabelCheckbox::GetCheckBoxColor() const { return boxColor; }

void UILabelCheckbox::SetCheckMarkColor(const SDL_Color& newColor) { checkColor = newColor; }

SDL_Color UILabelCheckbox::GetCheckMarkColor() const { return checkColor; }

void UILabelCheckbox::SetBoxSize(float newSize) { boxSize = newSize; }

void UILabelCheckbox::OverrideBoxSize(float newSize) { originalBoxSize = boxSize = newSize; }

float UILabelCheckbox::GetBoxSize() const { return boxSize; }

void UILabelCheckbox::SetSpacing(float newSpacing) { spacing = newSpacing; }

float UILabelCheckbox::GetSpacing() const { return spacing; }

void UILabelCheckbox::SetChecked(CheckboxState newState) { checkedFlag = newState; }

CheckboxState UILabelCheckbox::IsChecked() const { return checkedFlag; }

void UILabelCheckbox::OnRender(SDL_Renderer* renderer) const
{
    UILabel::RenderOffset(renderer, SDL_FPoint { boxSize + spacing , 0 });

    SDL_FRect checkbox {
        bounds.x,
        bounds.y + (bounds.h - boxSize) / 2.0f,
        boxSize,
        boxSize
    };

    if (!SDL_SetRenderDrawColor(renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UILabelCheckbox.OnRender: Failed SetRenderDrawColor for %s: %s", text.c_str(), SDL_GetError());
    }

    if (!SDL_RenderFillRect(renderer, &checkbox))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UILabelCheckbox.OnRender: Failed RenderFillRect for %s: %s", text.c_str(), SDL_GetError());
    }

    if (checkedFlag)
    {
        SDL_FRect check {
            checkbox.x + 4.0f,
            checkbox.y + 4.0f,
            checkbox.w - 8.0f,
            checkbox.h - 8.0f
        };

        if (!SDL_SetRenderDrawColor(renderer, checkColor.r, checkColor.g, checkColor.b, checkColor.a))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UILabelCheckbox.OnRender: Failed SetRenderDrawColor for %s: %s", text.c_str(), SDL_GetError());
        }

        if (!SDL_RenderFillRect(renderer, &check))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UILabelCheckbox.OnRender: Failed RenderFillRect for %s: %s", text.c_str(), SDL_GetError());
        }
    }


}
