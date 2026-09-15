#pragma once

#ifndef UILABELCHECKBOX_H_
#define UILABELCHECKBOX_H_

#include <functional>

#include "UILabel.h"

typedef bool CheckboxState;
constexpr CheckboxState CHECKBOX_CHECKED = true;
constexpr CheckboxState CHECKBOX_UNCHECKED = false;

class UILabelCheckbox : public UILabel
{
private:
    CheckboxState checkedFlag = CHECKBOX_UNCHECKED; /// Flag if box is checked or not.
    std::function<void(CheckboxState)> onToggle;    /// Function to call on toggle.

    SDL_Color boxColor = { 200, 200, 200, 255 };    /// Color of the box.
    SDL_Color checkColor = { 50, 200, 50, 255 };    /// Color of the check mark.

    float originalBoxSize = 20.0f;  /// Original box size for resizing.
    float boxSize = 20.0f;  /// Size of the box.
    float spacing = 10.0f;  /// Spacing.

protected:

    /**
    * @brief Renders the label checkbox.
    *
    * @param renderer Pointer to the SDL Renderer used.
    */
    void OnRender(SDL_Renderer* renderer) const override;

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
    * @param boxColor Color of the check box.
    * @param checkColor Color of the check mark.
    * @param initialState Initial state of the check box.
	* @param boxSize Size of the check box.
	* @param spacing Spacing between the check box and the text.
    */
    explicit UILabelCheckbox(
        const std::string id = "",
        const SDL_FRect& bounds = { 0.0f, 0.0f, 0.0f, 0.0f },
        const SDL_Color& backgroundColor = { 0, 0, 0, 0 },
        const SDL_Color& borderColor = { 255, 255, 255, 255 },
        const std::string& text = "",
        TTF_Font* textFont = nullptr,
        const SDL_Color& textColor = { 255, 255, 255, 255 },
        const UITextAlignment& textAlignment = { HorizontalAlignment::Center, VerticalAlignment::Middle },
        const std::string& fontFamily = "",
        const SDL_Color& boxColor = { 200, 200, 200, 255 },
        const SDL_Color& checkColor = { 50, 200, 50, 255 },
        CheckboxState initialState = CHECKBOX_UNCHECKED,
		float boxSize = 20.0f,
		float spacing = 10.0f
    );

    /**
    * @brief Handles an event.
    * 
    * @param e Event to be handled.
    */
    void HandleEvent(const SDL_Event& e) override;

    /**
    * @brief Resizes the label.
    *
    * @param widthScale The scale of the width (newWidth / oldWidth).
    * @param heightScale The scale of the height (newHeight / oldHeight).
    */
    void Resize(float widthScale, float heightScale) override;

    /**
    * @brief Sets the function to call on toggle.
    * 
    * @param callback Function to call when the checkbox gets toggled.
    */
    void SetOnToggle(const std::function<void(CheckboxState)>& callback);

    /**
    * @brief Sets the box color.
    * 
    * @param newColor New color for the box.
    */
    void SetCheckBoxColor(const SDL_Color& newColor);

    /**
    * @brief Returns the box color.
    * 
    * @return The box color.
    */
    SDL_Color GetCheckBoxColor() const;

    /**
    * @brief Sets the check mark color.
    *
    * @param newColor New color for the check mark.
    */
    void SetCheckMarkColor(const SDL_Color& newColor);

    /**
    * @brief Returns the check mark color.
    *
    * @return The check mark color.
    */
    SDL_Color GetCheckMarkColor() const;

    /**
	* @brief Sets the size of the box.
    * 
	* @param newSize New size for the box.
    */
	void SetBoxSize(float newSize);

    /**
    * @brief Sets the size of the box, including overriding the original box size.
    *
    * @param newSize New size for the box.
    */
    void OverrideBoxSize(float newSize);

    /**
	* @brief Returns the size of the box.
    * 
	* @return The size of the box.
    */
	float GetBoxSize() const;

    /**
	* @brief Sets the spacing between the box and the text.
    * 
	* @param newSpacing New spacing between the box and the text.
    */
	void SetSpacing(float newSpacing);

    /**
	* @brief Returns the spacing between the box and the text.
    * 
	* @return The spacing between the box and the text.
    */
	float GetSpacing() const;

    /**
    * @brief Sets the checked state.
    * 
    * @param newState New state for the checked flag.
    */
    void SetChecked(CheckboxState newState);

    /**
    * @brief Returns the state of the checkbox.
    * 
    * @return 'true' / CHECKBOX_CHECKED if the check box is checked, 'false' / CHECKBOX_UNCHECKED if otherwise.
    */
    CheckboxState IsChecked() const;
};


#endif // UILABELCHECKBOX_H_
