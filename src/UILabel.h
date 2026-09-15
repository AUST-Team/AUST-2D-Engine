#pragma once

#ifndef UILABEL_H_
#define UILABEL_H_

#include "UIElement.h"
#include "UICachedTextElement.h"

class UILabel : public UIElement, public UICachedTextElement 
{
private:

    // No copying allowed due to the cached texture.
    UILabel(const UILabel&) = delete;
    UILabel& operator=(const UILabel&) = delete;

protected:

    /**
    * @brief Renders the label on screen.
    *
    * @param renderer The renderer used.
    */
    virtual void OnRender(SDL_Renderer* renderer) const override;

public:
    
    /**
    * @brief Constructor.
    * 
    * @param id ID of the element.
    * @param bounds The bounds (coordinates and size) of the label.
    * @param backgroundColor The color of the background.
    * @param borderColor The color of the border.
    * @param text The text of the label.
    * @param textFont The font of the text.
    * @param textColor The color of the text.
    * @param textAlignment Alignment of the text.
	* @param fontFamily Font family name of the text font.
    */
    explicit UILabel(
        const std::string id = "",
        const SDL_FRect& bounds = {0.0f, 0.0f, 0.0f, 0.0f},
        const SDL_Color& backgroundColor = { 0, 0, 0, 0 },
        const SDL_Color& borderColor = { 255, 255, 255, 255 },
        const std::string& text = "",
        TTF_Font* textFont = nullptr, 
        const SDL_Color& textColor = { 255, 255, 255, 255 },
        const UITextAlignment& textAlignment = { HorizontalAlignment::Center, VerticalAlignment::Middle },
        const std::string& fontFamily = ""
    );
    
    /**
    * @brief Default destructor.
    */
    virtual ~UILabel() override = default;

    /**
    * @brief Default move constructor.
    */
    UILabel(UILabel&& other) noexcept = default;

    /**
	* @brief Updates the label (animations, etc).
    * 
	* @param deltaTime The delta time of the main SDL loop (ms).
    */
	virtual void Update(double deltaTime) override;

    /**
	* @brief Handles an event.
    * 
	* @param e The SDL_Event to be handled.
    */
	virtual void HandleEvent(const SDL_Event& e) override;

    /**
    * @brief Resizes the label.
    *
    * @param widthScale The scale of the width (newWidth / oldWidth).
    * @param heightScale The scale of the height (newHeight / oldHeight).
    */
	virtual void Resize(float widthScale, float heightScale) override;

    /**
    * @brief Renders the label TEXT on screen with an offset.
    * 
    * \/!\ ONLY SHIFTS THE TEXT, DOES NOT RENDER THE BACKGROUND \/!\
    * 
    * @param renderer Pointer to the SDL Renderer used.
    * @param offset SDL FPoint of the offsets.
    */
    virtual void RenderOffset(SDL_Renderer* renderer, const SDL_FPoint& offset) const;

    /**
    * @brief Sets the bounds of the label.
    * 
    * Invalidates the cache.
    * 
    * @param newBounds New bounds of the elements.
    */
    virtual void SetBounds(const SDL_FRect& newBounds) override;

    /**
    * @brief Default move operator.
    */
    UILabel& operator=(UILabel&& other) noexcept = default;
};

#endif
