#include <SDL3/SDL.h>

#include "UILabelPool.h"

void UILabelPool::Render(SDL_Renderer* renderer, float yOffset) const
{
    if (yOffset == 0.0f)
    {
        for (const UILabel& label : labels)
        {
            label.Render(renderer);
        }
        return;
    }

    for (UILabel& label : labels)
    {
        label.RenderOffset(renderer, SDL_FPoint { 0, yOffset });
    }
}

void UILabelPool::Resize(size_t newSize) { labels.resize(newSize); }

void UILabelPool::Clear() { labels.clear(); }

bool UILabelPool::Empty() const { return labels.empty(); }

void UILabelPool::InvalidateCache()
{
    for (UILabel& label : labels)
    {
        label.InvalidateCache();
    }
}

void UILabelPool::SetTextFont(TTF_Font* newFont)
{
    for (UILabel& label : labels)
    {
        label.SetTextFont(newFont);
    }
}

void UILabelPool::SetTextColor(const SDL_Color& newColor)
{
    for (UILabel& label : labels)
    {
        label.SetTextColor(newColor);
    }
}

void UILabelPool::SetBorderColor(const SDL_Color& newColor)
{
    for (UILabel& label : labels)
    {
        label.SetBorderColor(newColor);
    }
}

void UILabelPool::SetTextAlignment(UITextAlignment newAlignment)
{
    for (UILabel& label : labels)
    {
        label.SetTextAlignment(newAlignment);
    }
}

size_t UILabelPool::Size() const { return labels.size(); }

std::vector<UILabel>& UILabelPool::Get() { return labels; }

const std::vector<UILabel>& UILabelPool::Get() const { return labels; }

UILabel& UILabelPool::operator[](size_t index) { return labels[index]; }

const UILabel& UILabelPool::operator[](size_t index) const { return labels[index]; }
