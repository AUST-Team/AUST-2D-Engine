#pragma once

#ifndef UIELEMENT_H_
#define UIELEMENT_H_

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_pixels.h>
#include <string>

struct SDL_Renderer;
union SDL_Event;

typedef bool BorderVisibility;
constexpr BorderVisibility SHOW_BORDERS = true;
constexpr BorderVisibility HIDE_BORDERS = false;

typedef bool ElementVisiblity;
constexpr ElementVisiblity SHOW_ELEMENT = true;
constexpr ElementVisiblity HIDE_ELEMENT = false;

/*
* Now you may wonder: "Why didn't you just use an already existing GUI library?".
* Well I tried, wanted RmlUI, but it just didn't want to work, whenever I tried to get the SDL + GL3 background working, it would throw and std::out_of_range error, so I am making my own.
*/

/**
* @brief Abstract class for an UI element, any concrete UI element must inherit.
*/
class UIElement 
{
protected:
    std::string id; /// Element ID.
	SDL_FRect initialBounds = { 0.0f, 0.0f, 0.0f, 0.0f };   /// Initial bounds of the element, used for resizing.
    SDL_FRect bounds = { 0.0f, 0.0f, 0.0f, 0.0f };          /// Bounds of the element.
    SDL_Color backgroundColor = { 0, 0, 0, 0 };             /// Background color of the element.
    SDL_Color borderColor = { 255, 255, 255, 255 };         /// Border color of the element.

    ElementVisiblity visibleFlag = SHOW_ELEMENT;        /// Flag if the element should be visible or not.
    BorderVisibility showBordersFlag = HIDE_BORDERS;    /// Flag if the borders should be shown.

    /**
     * @brief Render callback for derived classes.
     *
     * Called only if the element is visible. Use this to implement
     * rendering logic in sub-classes instead of overriding Render().
     *
     * @param renderer Pointer to the SDL renderer used for drawing.
     */
    virtual void OnRender(SDL_Renderer* renderer) const = 0;

public:

    /**
    * @brief Constructor.
    * 
    * @param id ID of the element.
    * @param bounds Bounds of the element.
    * @param backgroundColor Background color of the element.
    * @param borderColor Border color of the element.
    */
    UIElement(
        const std::string& id = "",
        const SDL_FRect& bounds = { 0.0f, 0.0f, 0.0f, 0.0f },
        const SDL_Color& backgroundColor = { 0, 0, 0, 0 },
        const SDL_Color& borderColor = { 255, 255, 255, 255 }
    );
    
    /**
    * @brief Default destructor.
    */
    virtual ~UIElement() = default;

    /**
     * @brief Renders the element.
     *
     * Requires manual checking of visibility flag before calling OnRender().
     * You should override OnRender() in subclasses, not this.
     *
     * @param renderer Pointer to the SDL renderer used for drawing.
     */
    virtual void Render(SDL_Renderer* renderer) const;
    
    /**
    * @brief Handles an event.
    * 
    * @param e The SDL_Event to be handled.
    */
    virtual void HandleEvent(const SDL_Event& e) = 0;

    /**
    * @brief Updates the UI elemement (animations, etc).
    * 
    * @param deltaTile The delta time of the main SDL loop (ms).
    */
    virtual void Update(double deltaTime) = 0;

    /**
	* @brief Resizes the element.
    * 
	* @param widthScale The scale of the width (newWidth / oldWidth).
	* @param heightScale The scale of the height (newHeight / oldHeight).
    */
    virtual void Resize(float widthScale, float heightScale) = 0;

    /**
    * @brief Sets the ID of the element.
    * 
    * @param newId New ID of the element.
    */
    virtual void SetId(const std::string& newId);

    /**
    * @brief Returns the ID of the element.
    * 
    * @return Reference to the ID of the element.
    */
    virtual std::string& GetId();

    /**
    * @brief Returns the ID of the element.
    * 
    * @return Constant reference to the ID of the element.
    */
    virtual const std::string& GetId() const;

    /**
    * @brief Sets the position of the elemement (x, y, width, height).
    * 
	* Does not override the initial bounds, so resizing will still be based on the original bounds.
    * 
    * @param newBounds The SDL_FRect of the new bounds.
    */
    virtual void SetBounds(const SDL_FRect& newBounds);

    /**
	* @brief Sets the position of the element, overriding the initial bounds for resizing.
    * 
	* @param newBounds The SDL_FRect of the new bounds.
    */
	virtual void OverrideBounds(const SDL_FRect& newBounds);

    /**
    * @brief Returns the bounds of the element.
    *
    * @return The bounds of the element.
    */
    virtual SDL_FRect GetBounds() const;

    /**
    * @brief Sets the background color of the element.
    * 
    * @param newColor New background color.
    */
    virtual void SetBackgroundColor(const SDL_Color& newColor);

    /**
    * @brief Returns the background color of the element.
    *
    * @return The background color of the element.
    */
    virtual SDL_Color GetBackgroundColor() const;

    /**
    * @brief Sets the border color of the element.
    * 
    * @param newColor New border color.
    */
    virtual void SetBorderColor(const SDL_Color& newColor);

    /**
    * @brief Returns the border color of the element.
    *
    * @return The border color of the element.
    */
    virtual SDL_Color GetBorderColor() const;

    /**
    * @brief Sets the visibility of the element.
    * 
    * @param newValue 'SHOW_ELEMENT / true' if the element should be visible, 'HIDE_ELEMENT / false' if otherwise.
    */
    virtual void SetVisible(ElementVisiblity newValue);

    /**
    * @brief Checks if the element is visible or not.
    * 
    * @return 'SHOW_ELEMENT / true' if the element is visible, 'HIDE_ELEMENT / false' if otherwise.
    */
    virtual ElementVisiblity IsVisible() const;

    /**
    * @brief Sets the visibility of the border.
    *
    * @param newValue 'SHOW_BORDERS' (true) if the border should be visible, 'HIDE_BORDERS' (false) if otherwise.
    */
    virtual void SetBorderVisible(BorderVisibility newValue);

    /**
    * @brief Checks if the element is visible or not.
    *
    * @return 'SHOW_BORDERS / true' if the element is visible, 'HIDE_BORDERS / false' if otherwise.
    */
    virtual BorderVisibility IsBorderVisible() const;

    /**
    * @brief Determines if a given point on the screen is inside the bounds.
    * 
    * @param x X coordinate of the point.
    * @param y Y coordinate of the point.
    * 
    * @return true if the point is inside the element, false otherwise.
    */
    virtual bool Contains(float x, float y) const;

    /**
    * @brief Determines if a given point on the screen is inside the bounds.
    *
    * @param point A floating point structure of the point's position.
    *
    * @return true if the point is inside the element, false otherwise.
    */
    virtual bool Contains(const SDL_FPoint& point) const;
};

#endif // UIELEMENT_H_
