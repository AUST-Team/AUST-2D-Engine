#include <SDL3/SDL.h>

#include "UIButton.h"

#include "Configuration.h"
#include "SDL_ColorOperators.h"
#include "Audio.h"

UIButton::UIButton(const std::string id, const SDL_FRect& bounds, const SDL_Color& backgroundColor, const SDL_Color& borderColor, const std::string& text, TTF_Font* textFont, const SDL_Color& textColor, const UITextAlignment& textAlignment, const std::string& fontFamily) :
    UILabel(id, bounds, backgroundColor, borderColor, text, textFont, textColor, textAlignment, fontFamily) {}

void UIButton::HandleEvent(const SDL_Event& e)
{
    if (!isActiveFlag)
    {
        return;
    }

    if (e.type == SDL_EVENT_MOUSE_MOTION)
    {
        SDL_FPoint mousePoint = { static_cast<float>(e.button.x), static_cast<float>(e.button.y) };

        const bool inside = Contains(mousePoint);

        if (inside && !isHoveredFlag)
        {
            isHoveredFlag = true;
            TriggerCallbacks(ButtonCallbackType::EnterHover);
        }
        else if (!inside && isHoveredFlag)
        {
            isHoveredFlag = false;
            TriggerCallbacks(ButtonCallbackType::ExitHover);
        }
    }

    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == SDL_BUTTON_LEFT)
    {
        SDL_FPoint mousePoint = { static_cast<float>(e.button.x), static_cast<float>(e.button.y) };

        if (Contains(mousePoint))
        {
            TriggerCallbacks(ButtonCallbackType::Click);
        }
    }
}

void UIButton::AddCallback(const std::function<void()>& callback, ButtonCallbackType type) 
{ 
    if (!callback)
    {
        return;
    }

    callbacksByType[ToIndex(type)].push_back(callback);
}

void UIButton::SetActive(bool newValue) 
{ 
    if (isActiveFlag == newValue)
    {
        return;
    }

    isActiveFlag = newValue;

    if (!isActiveFlag)
    {
        cachedEnabledColor = textColor;
        SetTextColor(Configuration::Get().ui.disabled);
    }
    else
    {
        SetTextColor(cachedEnabledColor);
    }
}

bool UIButton::GetActiveStatus() const { return isActiveFlag; }

void UIButton::TriggerCallbacks(ButtonCallbackType type)
{
    for (const std::function<void()>& callback : callbacksByType[ToIndex(type)])
    {
        if (callback)
        {
            callback();
        }
    }
}
