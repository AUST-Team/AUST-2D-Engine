#include <SDL3/SDL.h>

#include "UIPanel.h"
#include "MemoryTracker.h"

UIPanel::UIPanel(const std::string id, const SDL_FRect& bounds, const SDL_Color& backgroundColor, const SDL_Color& borderColor, const std::vector<UIElement*>& elementsVec, const std::array<std::vector<std::function<void()>>, enumCount<PanelCallbackType>> callbacksArr) :
    UIElement(id, bounds, backgroundColor, borderColor),
    callbacks(callbacksArr),
    elements(elementsVec) {}

UIPanel::~UIPanel() { Clear(); }

void UIPanel::AddElement(UIElement* element)
{
    if (!element)
    {
        return;
    }

    elements.push_back(std::move(element));
}

void UIPanel::HandleEvent(const SDL_Event& e)
{
    for (UIElement*& elem : elements)
    {
        if (elem)
        {
            elem->HandleEvent(e);
        }
    }
}

void UIPanel::Update(double deltaTime) 
{
    for (UIElement*& elem : elements) 
    {
        if (elem)
        {
            elem->Update(deltaTime);
        }
    }
}

void UIPanel::Resize(float widthScale, float heightScale)
{
    UIElement::Resize(widthScale, heightScale);

    for (UIElement*& elem : elements) 
    {
        if (elem)
        {
            elem->Resize(widthScale, heightScale);
        }
	}
}

void UIPanel::RenderPanel(SDL_Renderer* renderer) const 
{ 
    if (!visibleFlag)
    {
        return;
    }

    if (backgroundColor.a == 0)
    {
        return;
    }

    if (!SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIPanel.RenderPanel: SetRenderDrawColor failed: %s", SDL_GetError());
    }

    if (!SDL_RenderFillRect(renderer, &bounds))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIPanel.RenderPanel: RenderFillRect failed: %s", SDL_GetError());
    }
}

void UIPanel::Clear()
{ 
    for (UIElement*& elem : elements)
    {
        if (elem)
        {
            ENG_DELETE(elem);
            elem = nullptr;
        }
    }
    elements.clear(); 
}

std::vector<UIElement *>& UIPanel::GetElements() { return elements; }

const std::vector<UIElement *>& UIPanel::GetElements() const { return elements; }

void UIPanel::AddCallback(std::function<void()> callback, PanelCallbackType type)
{
    if(!callback)
    { 
        return;
    }

    callbacks[ToIndex(type)].push_back(callback);
}

void UIPanel::TriggerCallbacks(PanelCallbackType type)
{
    for (const std::function<void()>& callback : callbacks[ToIndex(type)])
    {
        if (callback)
        {
            callback();
        }
    }
}

void UIPanel::OnRender(SDL_Renderer* renderer) const
{
    for (UIElement* elem : elements)
    {
        if (elem)
        {
            elem->Render(renderer);
        }
    }
}

