#include <SDL3/SDL.h>

#include "UIElement.h"

#include "Unused.h"

UIElement::UIElement(const std::string& id, const SDL_FRect& bounds, const SDL_Color& backgroundColor, const SDL_Color& borderColor) :
    id(id),
    initialBounds(bounds),
    backgroundColor(backgroundColor),
    bounds(bounds),
    borderColor(borderColor),
    showBordersFlag(borderColor.a != 0 ? SHOW_BORDERS : HIDE_BORDERS) {}

void UIElement::Render(SDL_Renderer* renderer) const
{
    if (!visibleFlag)
    {
        return;
    }

    if (backgroundColor.a != 0)
    {
        if (!SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIElement.Render: SetRenderDrawColor failed: %s", SDL_GetError());
        }

        if (!SDL_RenderFillRect(renderer, &bounds))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIElement.Render: RenderFillRect failed: %s", SDL_GetError());
        }
    }

    OnRender(renderer);

    if (showBordersFlag && borderColor.a != 0)
    {
        if (!SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIElement.Render: SetRenderDrawColor failed: %s", SDL_GetError());
        }

        if (!SDL_RenderRect(renderer, &bounds))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIElement.Render: RenderRect failed: %s", SDL_GetError());
        }
    }
}

void UIElement::HandleEvent(const SDL_Event& e) { UNUSED(e); }

void UIElement::Update(double deltaTime) { UNUSED(deltaTime); }

void UIElement::Resize(float widthScale, float heightScale)
{
    bounds.x = initialBounds.x * widthScale;
    bounds.y = initialBounds.y * heightScale;
    bounds.w = initialBounds.w * widthScale;
    bounds.h = initialBounds.h * heightScale;
}

void UIElement::SetId(const std::string& newId) { id = newId; }

std::string& UIElement::GetId() { return id; }

const std::string& UIElement::GetId() const { return id; }

void UIElement::SetBounds(const SDL_FRect& newBounds) { bounds = newBounds; }

void UIElement::OverrideBounds(const SDL_FRect& newBounds)
{
	bounds = newBounds; 
    initialBounds = newBounds;
}

SDL_FRect UIElement::GetBounds() const { return bounds; }

void UIElement::SetBackgroundColor(const SDL_Color& newColor) { backgroundColor = newColor; }

SDL_Color UIElement::GetBackgroundColor() const { return backgroundColor; }

void UIElement::SetBorderColor(const SDL_Color& newColor) { borderColor = newColor; }

SDL_Color UIElement::GetBorderColor() const { return borderColor; }

void UIElement::SetVisible(ElementVisiblity newValue) { visibleFlag = newValue; }

ElementVisiblity UIElement::IsVisible() const { return visibleFlag; }

void UIElement::SetBorderVisible(BorderVisibility newValue) { showBordersFlag = newValue; }

BorderVisibility UIElement::IsBorderVisible() const { return showBordersFlag; }

bool UIElement::Contains(float x, float y) const
{
    SDL_FPoint point { x, y };
    return SDL_PointInRectFloat(&point, &bounds);
}

bool UIElement::Contains(const SDL_FPoint& point) const
{
    return SDL_PointInRectFloat(&point, &bounds);
}
