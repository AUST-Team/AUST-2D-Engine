#pragma once

#ifndef UIBUTTON_H_
#define UIBUTTON_H_

#include <functional>
#include <vector>
#include <array>

#include "UILabel.h"
#include "ButtonCallbackType.h"

/**
 * @brief Basic UI button that can be rendered and clicked.
*/
class UIButton : public UILabel
{
private:

    /// An array of vectors of callbacks by type.
    std::array<
        std::vector<
            std::function<void()>
        >, 
        enumCount<ButtonCallbackType>
    > callbacksByType;

    SDL_Color cachedEnabledColor = { 255, 255, 255, 255 };  /// The cached enabled color.
    bool isHoveredFlag = false;     /// Flag if the button is hovered over.
    bool isActiveFlag = true;       /// Flag if button is active or not.

    // No copying allowed due to cached texture (UILabel).
    UIButton(const UIButton&) = delete;
    UIButton& operator=(const UIButton&) = delete;

    /**
    * @brief Triggers all callbacks of a certain type.
    * 
    * @param type Type of callbacks to trigger.
    */
    void TriggerCallbacks(ButtonCallbackType type);

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
    explicit UIButton(
        const std::string id = "",
        const SDL_FRect& bounds = { 0.0f, 0.0f, 0.0f, 0.0f },
        const SDL_Color& backgroundColor = { 0, 0, 0, 0 },
        const SDL_Color& borderColor = { 255, 255, 255, 255 },
        const std::string& text = "",
        TTF_Font* textFont = nullptr,
        const SDL_Color& textColor = { 255, 255, 255, 255 },
        const UITextAlignment& textAlignment = { HorizontalAlignment::Center, VerticalAlignment::Middle },
		const std::string& fontFamily = ""
    );

    /**
    * @brief Destructor.
    */
    ~UIButton() override = default;

    /**
    * @brief Default move constructor.
    */
    UIButton(UIButton&& other) noexcept = default;

    /**
    * @brief Handles an event.
    *
    * @param Event to be handled.
    */
    void HandleEvent(const SDL_Event& e) override;

    /**
    * @brief Adds the function to the vector of callbacks.
    * 
    * @param callback The callback.
    * @param type Type of callback.
    */
    void AddCallback(const std::function<void()>& callback, ButtonCallbackType type);

    /**
    * @brief Sets if the button should be active or not.
    * 
    * @param newValue 'true' if the button should be active, 'false' if otherwise.
    */
    void SetActive(bool newValue);

    /**
    * @brief Checks if the button is active or not.
    * 
    * @return 'true' if the button is active, false if otherwise.
    */
    bool GetActiveStatus() const;

    /**
    * @brief Default move operator.
    */
    UIButton& operator=(UIButton&& other) noexcept = default;
};


#endif // UIBUTTON_H_