#pragma once

#ifndef UITEXTFIELD_H_
#define UITEXTFIELD_H_

#include <array>
#include <functional>
#include <vector>

#include "UIElement.h"
#include "UITextElement.h"
#include "TextFieldCallbackType.h"
#include "SliderLayout.h"
#include "Size.h"

/**
* @brief UI element for a text field.
*/
class UITextField : public UIElement, public UITextElement
{
private:
    std::array<std::vector<std::function<void()>>, enumCount<TextFieldCallbackType>> callbacks;  /// Callbacks for the different input types.

    std::string placeholder;    /// Placeholder text.
    size_t maxLength = 32;      /// Maximum lenght of text.

    float blinkTimer = 0.0f;            /// Timer for blinking.
    float timeForBlink = 500.0f;        /// Time between blinks.
    mutable float scrollOffset = 0.0f;  /// Offset for horizontal scrolling.
    float textPadding = 5.0f;           /// Pading between the edges of the text box and the text.

    SDL_Color cursorColor = { 255, 255, 255, 255 };     /// Color of the cursor.
    SDL_Color sliderTrackColor = { 80, 80, 80, 255 };   /// Color of the slider track (background).
    SDL_Color sliderActiveColor = { 0, 180, 255, 255 }; /// Color of the slider track active color (foreground).
    SDL_Color handleColor = { 240, 240, 240, 255 };     /// Color of the handle.

    float sliderPadding = 10.0f;            /// Gap between TextField and Slider.
    FSize sliderSize = { 120.0f, 4.0f };    /// Size of the slider.
    FSize handleSize = { 10.0f, 16.0f };    /// Size of the handle.

    FSize originalSliderSize = { 120.0f, 4.0f };    /// Original size of the slider.
    FSize originalHandleSize = { 10.0f, 16.0f };    /// Original size of the handle.

    bool showCursorFlag = true;     /// Flag if the cursor should be shown.
    bool focusedFlag = false;       /// Flag if the text field has focus.
    bool numericOnlyFlag = false;   /// Flag if the text field is numeric only.
    bool showSliderFlag = false;    /// Flag if the text field should show a slider. If 'numericOnlyFlag' is false, it will not show a slider, even if this is set to 'true'.

    /**
    * @brief Starts the text input.
    */
    void StartTextInput();

    /**
    * @brief Stops the text input.
    */
    void StopTextInput();

    /**
    * @brief Triggers all callbacks of a certain type.
    * 
    * @param type Type of callbacks (text input) to trigger.
    */
    void TriggerCallbacks(TextFieldCallbackType type);

    /**
    * @brief Returns the slider layout for a text field with a slider.
    * 
    * @param progress Progress of the slider.
    * 
    * @return The slider layout.
    */
    SliderLayout GetSliderLayout(float progress) const;

protected:

    /**
    * @brief Renders the text field.
    *
    * @param rendere Pointer to the SDL_Renderer used.
    */
    void OnRender(SDL_Renderer* renderer) const override;

public:


    // Oh boy. And in my software engineering classes they told me that a function with too many parameters is a bad sign.
    // Weirdly enough I prefer to use the constructor, rather than make an instance and use 500000 setters.
    /**
    * @brief Constructor.
    * 
    * @param id ID of the element.
    * @param bounds Bounds of the text field.
    * @param backgroundColor Background color of the text field.
    * @param borderColor Border color of the text field.
    * @param text Text in the field box.
	* @param textFont The font of the text.
	* @param textColor Color of the text.
	* @param textAlignment Alignment of the text.
	* @param fontFamily Font family name of the text font.
    * @param paceholderText Placeholder text for the text field, will be used if no text is provided.
    * @param maxLength Maximum lenght of the text.
    * @param blinkTime Blink time of the cursor.
    * @param numericOnly Flag if the text field should be numeric only.
    * @param showSlider Flag if a slider should be shown. Requires the 'numericOnly' flag to be 'true'. Slider will be to the RIGHT of the text box.
    * @param textPadding Padding of the text from the edges of the text box.
    * @param cursorColor Color of the cursor.
    * @param sliderTrackColor Color of the slider track (background).
    * @param sliderActiveColor Color of the slider's active track (foreground).
    * @param handleColor Color of the handle.
    * @param sliderPadding Padding of the slider from the text box.
    * @param sliderSize The size of the slider.
    * @param handleSize The size of the handle.
    * @param callbacksByType Array of callbacks for the text input types.
    */
    explicit UITextField(
        const std::string id = "",
        const SDL_FRect& bounds = { 0.0f, 0.0f, 0.0f, 0.0f },
        const SDL_Color& backgroundColor = { 30, 30, 30, 255 },
        const SDL_Color& borderColor = { 255, 255, 255, 255 },
        const std::string& text = "",
        TTF_Font * textFont = nullptr,
        const SDL_Color& textColor = { 255, 255, 255, 255 },
        const UITextAlignment& textAlignment = { HorizontalAlignment::Center, VerticalAlignment::Middle },
        const std::string& fontFamily = "",
        const std::string& placeholderText = "",
        size_t maxLength = 32,
        float blinkTime = 500.0f,
        bool numericOnly = false,
        bool showSlider = false,
        float textPadding = 5.0f,
        SDL_Color cursorColor = { 255, 255, 255, 255 },
        std::array<std::vector<std::function<void()>>, enumCount<TextFieldCallbackType>> callbacksByType = {},
        SDL_Color sliderTrackColor = { 80, 80, 80, 255 },
        SDL_Color sliderActiveColor = { 0, 180, 255, 255 },
        SDL_Color handleColor = { 240, 240, 240, 255 },
        float sliderPadding = 10.0f,
        FSize sliderSize = { 120.0f, 4.0f },
        FSize handleSize = { 10.0f, 16.0f }
    );

    /**
    * @brief Default destructor.
    */
    ~UITextField() override = default;

    /**
    * @brief Handles an event.
    * 
    * @param e Event to be handled.
    */
    void HandleEvent(const SDL_Event& e) override;

    /**
    * @brief Updates the text field.
    * 
    * @param deltaTime The delta time of the main game loop.
    */
    void Update(double deltaTime) override;

    /**
    * @brief Resizes the element.
    *
    * @param widthScale The scale of the width (newWidth / oldWidth).
    * @param heightScale The scale of the height (newHeight / oldHeight).
    */
    void Resize(float widthScale, float heightScale) override;

    /**
    * @brief Adds a callback.
    * 
    * @param callback Callback to add.
    * @param type Type of callback (when text starts / stops being inputted).
    */
    void AddCallback(const std::function<void()> callback, TextFieldCallbackType type);

    /**
    * @brief Sets the placeholder text.
    * 
    * @param newPlaceholder The new text for the placeholder.
    */
    void SetPlaceholderText(const std::string& newPlaceholder);

    /**
    * @brief Returns the placeholder text.
    * 
    * @return The placeholder text.
    */
    std::string GetPlaceholderText() const;

    /**
    * @brief Sets the maximum lenght for the text.
    * 
    * @param newLength The new length of the text.
    */
    void SetMaxTextLength(size_t newLength);

    /**
    * @brief Returns the max length of the text.
    * 
    * @return Max length of the text.
    */
    size_t GetMaxTextLength() const;

    /**
    * @brief Sets whether the text field is numeric only.
    *
    * @param numericOnly Flag indicating if only numeric input is allowed.
    */
    void SetNumericOnlyFlag(bool numericOnly);

    /**
    * @brief Returns whether the text field is numeric only.
    *
    * @return True if numeric only, false otherwise.
    */
    bool GetNumericOnlyFlag() const;

    /**
    * @brief Sets whether to show the slider.
    *
    * @param showSlider Flag indicating if the slider should be visible.
    */
    void SetShowSliderFlag(bool showSlider);

    /**
    * @brief Returns whether the slider is visible.
    *
    * @return True if the slider is shown, false otherwise.
    */
    bool GetShowSliderFlag() const;

    /**
    * @brief Returns the text as an integer. Only works if the 'numericOnly' flag has been set.
    * 
    * @return The value of the text, as an integer, or -1 if the 'numericOnly' flag is 'false', or if the text cannot be parsed.
    */
    int GetTextAsInteger() const;

    /**
    * @brief Sets the padding between the edges of the text box and the text.
    *
    * @param newPadding The new text padding value.
    */
    void SetTextPadding(float newPadding);

    /**
    * @brief Returns the padding between the edges of the text box and the text.
    *
    * @return The text padding value.
    */
    float GetTextPadding() const;

    /**
    * @brief Sets the color of the cursor.
    *
    * @param newColor The new cursor color.
    */
    void SetCursorColor(const SDL_Color& newColor);

    /**
    * @brief Returns the color of the cursor.
    *
    * @return The cursor color.
    */
    SDL_Color GetCursorColor() const;

    /**
    * @brief Sets the background color of the slider track.
    *
    * @param newColor The new slider track color.
    */
    void SetSliderTrackColor(const SDL_Color& newColor);

    /**
    * @brief Returns the background color of the slider track.
    *
    * @return The slider track color.
    */
    SDL_Color GetSliderTrackColor() const;

    /**
    * @brief Sets the active (foreground) color of the slider track.
    *
    * @param newColor The new active slider track color.
    */
    void SetSliderActiveColor(const SDL_Color& newColor);

    /**
    * @brief Returns the active (foreground) color of the slider track.
    *
    * @return The active slider track color.
    */
    SDL_Color GetSliderActiveColor() const;

    /**
    * @brief Sets the color of the slider handle.
    *
    * @param newColor The new handle color.
    */
    void SetHandleColor(const SDL_Color& newColor);

    /**
    * @brief Returns the color of the slider handle.
    *
    * @return The handle color.
    */
    SDL_Color GetHandleColor() const;

    /**
    * @brief Sets the gap between the TextField and the Slider.
    *
    * @param newPadding The new slider padding value.
    */
    void SetSliderPadding(float newPadding);

    /**
    * @brief Returns the gap between the TextField and the Slider.
    *
    * @return The slider padding value.
    */
    float GetSliderPadding() const;

    /**
    * @brief Sets the size of the slider.
    *
    * @param newSize The new size of the slider.
    */
    void SetSliderSize(const FSize& newSize);

    /**
    * @brief Sets the size of the slider. Will also override the original size to this.
    *
    * @param newSize The new size of the slider.
    */
    void OverrideSliderSize(const FSize& newSize);

    /**
    * @brief Returns the size of the slider.
    *
    * @return The size of the slider.
    */
    FSize GetSliderSize() const;

    /**
    * @brief Sets the size of the slider handle.
    *
    * @param newSize The new size of the handle.
    */
    void SetHandleSize(const FSize& newSize);

    /**
    * @brief Sets the size of the slider handle. Will also override the original size to this.
    *
    * @param newSize The new size of the handle.
    */
    void OverrideHandleSize(const FSize& newSize);

    /**
    * @brief Returns the size of the slider handle.
    *
    * @return The size of the handle.
    */
    FSize GetHandleSize() const;
};

#endif // UITEXTFIELD_H_