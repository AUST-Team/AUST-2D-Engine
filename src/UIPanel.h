#pragma once

#ifndef UIPANEL_H_
#define UIPANEL_H_

#include <vector>
#include <SDL3/SDL_log.h>
#include <array>
#include <functional>

#include "UIElement.h"
#include "PanelCallbackType.h"

class UIPanel : public UIElement 
{
private:
    std::array<std::vector<std::function<void()>>, enumCount<PanelCallbackType>> callbacks; /// Callbacks by type (type -> vector of callbacks).
    std::vector<UIElement*> elements;   /// Vector of pointers of elements. Owning!

    // No copying due to the possibility of some UIElements not being copyable.
    UIPanel(const UIPanel&) = delete;
    UIPanel& operator=(const UIPanel&) = delete;

protected:

    /**
    * @brief Renders the panel and all the elements.
    *
    * @param renderer The renderer to be used.
    */
    virtual void OnRender(SDL_Renderer* renderer) const override;

public:

    /**
    * @brief Constructor.
    * 
    * @param id ID of the element.
    * @param bounds The bounds of the panel.
    * @param backgroundColor Background color of the panel.
    * @param borderColor Border color of the panel.
    * @param elementsVec Vector of elements. OWNING!
    * @param callbacks Array of vectors of callbacks.
    */
    explicit UIPanel(
        const std::string id = "",
        const SDL_FRect& bounds = {0.0f, 0.0f, 0.0f, 0.0f},
        const SDL_Color& backgroundColor = { 0, 0, 0, 0 },
        const SDL_Color& borderColor = { 255, 255, 255, 255 },
        const std::vector<UIElement*>& elementsVec = {},
        const std::array<std::vector<std::function<void()>>, enumCount<PanelCallbackType>> callbacksArr = {}
    );

    /**
    * @brief Default destructor.
    */
    virtual ~UIPanel() override;

    /**
    * @brief Default move constructor.
    */
    UIPanel(UIPanel&& other) = default;
    
    /**
    * @brief Add an element to the panel.
    * 
    * @param element Element to be added.
    */
    virtual void AddElement(UIElement* element);

    /**
    * @brief Handles an event. Sends the event to all elements in the panel.
    * 
    * @param e The event to be handled.
    */
    virtual void HandleEvent(const SDL_Event& e) override;

    /**
    * @brief Updates all the elements in the panel.
    * 
    * @param deltaTime the delta time of the main SDL loop (msd).
    */
    virtual void Update(double deltaTime) override;

    /**
	* @brief Resizes the panel and all the elements in it.
    * 
	* @param widthScale The scale of the width (newWidth / oldWidth).
	* @param heightScale The scale of the height (newHeight / oldHeight).
    */
    virtual void Resize(float widthScale, float heightScale) override;

    /**
    * @brief Renders only the panel color.
    * 
    * @param renderer Pointer to the renderer used.
    */
    virtual void RenderPanel(SDL_Renderer* renderer) const;

    /**
    * @brief Clears the panel of all elements.
    */
    virtual void Clear();

    /**
    * @brief Returns all elements inside the panel.
    *
    * @return A reference to the vector of elements.
    */
    virtual std::vector<UIElement *>& GetElements();

    /**
    * @brief Returns all elements inside the panel.
    * 
    * @return Constant reference to the vector of elements.
    */
    virtual const std::vector<UIElement *>& GetElements() const;

    /**
    * @brief Adds a callback to the panel.
    * 
    * @param callback Callback to add.
    * @param type Type of callback to add (when panel is entered, exited, etc).
    */
    virtual void AddCallback(std::function<void()> callback, PanelCallbackType type);

    /**
    * @brief Triggers all callbacks of a given type. Since the panel has no idea when it is being entered / exited, you must call these manually.
    * 
    * @param type Type of callbacks to trigger.
    */
    virtual void TriggerCallbacks(PanelCallbackType type);

    /**
    * @brief Searches and returns an element T by ID.
    * 
    * @param id ID of the element.
    * 
    * @return The element, casted to type T*, or nullptr, if no such element exists.
    */
    template <typename T>
    T* FindElementById(const std::string& id)
    {
        for (UIElement*& elem : elements)
        {
            if (elem && elem->GetId() == id)
            {
                return dynamic_cast<T*>(elem);
            }
        }
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UIPanel.FindElementById: No element found with id [%s].", id.c_str());
        return nullptr;
    }

    /**
    * @brief Default move operator.
    */
    UIPanel& operator=(UIPanel&& other) = default;
};

#endif // UIPANEL_H_