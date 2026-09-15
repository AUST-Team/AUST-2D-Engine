#pragma once

#ifndef UIDIALOGUE_H_
#define UIDIALOGUE_H_

#include <functional>

#include "UILabelPool.h"
#include "Direction.h"
#include "ConstantConfiguration.h"
#include "ArrayMiscs.h"
#include "DialogueArrow.h"

class DialogueState;
class Command;

typedef bool ArrowVisibility;
constexpr ArrowVisibility SHOW_ARROWS = true;
constexpr ArrowVisibility HIDE_ARROWS = false;

/**
* @brief Class for an UI dialogue element.
*/
class UIDialogue : public UIElement, public UITextStyleElement
{
private:
    DialogueState* currentState = nullptr;  /// Current state of the dialogue. OWNED.
    DialogueState* pendingState = nullptr;  /// Pending state of the next dialouge. OWNED.

    UILabelPool lineLabels; /// Label pool for the lines.

    std::function<void()> onDialogueEndCallback = nullptr;  /// Function to call on dialogue end.

    std::array<DialogueArrow, enumCount<Direction>> dialogueArrows;   /// Array of dialogue arrows.

    float typewriterDelay = 30.0f;          /// Typewriter delay between writing letters. Miliseconds.
    float blinkTimer = 0.0f;                /// Timer for the blinking arrow.
    float timeBetweenBlinks = 1000.0f;      /// Time between blinks. Miliseconds.
    float lineSpacing = 1.25f;              /// Spacing between the lines.
    float iconSize = 36.0f;                 /// Size of the arrow icons.
    SDL_FPoint padding = { 40.0f, 20.0f };  /// Padding from the edges of the screen.
    ArrowVisibility showArrowsFlag = HIDE_ARROWS;  /// Flag if the arrows should be shown or not.

    // No copying allowed due to... gonna be honest I don't want to deal with polymorhpic coyping.
    UIDialogue(const UIDialogue&) = delete;
    UIDialogue& operator=(const UIDialogue&) = delete;

    /**
    * @brief Refreshes the current state.
    */
    void RefreshCurrentState();

protected:

    /**
    * @brief Renders the dialogue UI.
    *
    * @param renderer Pointer to the renderer used.
    */
    void OnRender(SDL_Renderer* renderer) const override;

public:

    /**
    * @brief Constructor.
    * 
    * @param id ID of the element.
    * @param bounds Bounds of the dialogue box.
    * @param backgroundColor Color of the background of the dialogue box.
    * @param borderColor Border color of the dialogue box.
    * @param textFont Font of the text.
    * @param textColor Color of the text.
    * @param textAlignment Alignment of the text.
	* @param fontFamily Font family name of the text font.
    * @param dialogueArrows Array for the dialogue arrows.
    * @param blinkTime Time between arrow blinks. Miliseconds.
    * @param padding Padding from the borders of the screen.
    * @param lineSpacing Spacing between the lines.
    * @param iconSize The size of the arrow icons, if they should be displayed.
    */
    explicit UIDialogue(
        const std::string id = "",
        const SDL_FRect& bounds = { 0.0f, 0.0f, 0.0f, 0.0f },
        const SDL_Color& backgroundColor = { 0, 0, 0, 150 },
        const SDL_Color& borderColor = { 255, 255, 255, 255 },
        TTF_Font* textFont = nullptr,
        const SDL_Color& textColor = { 255, 255, 255, 255 },
        const UITextAlignment& textAlignment = { HorizontalAlignment::Center, VerticalAlignment::Middle },
        const std::string& fontFamily = "",
        const std::array<DialogueArrow, enumCount<Direction>>& dialogueArrows = {},
        float blinkTime = 1000.0f,
        const SDL_FPoint& padding = { 40.0f, 20.0f },
        float lineSpacing = 1.25,
        float iconSize = 36.0f
    );

    /**
    * @brief Move constructor.
    */
    UIDialogue(UIDialogue&& other) noexcept;

    /**
    * @brief Destructor.
    */
    ~UIDialogue() override;

    /**
    * @brief Handles an event.
    * 
    * @param e Event to be handled.
    */
    void HandleEvent(const SDL_Event& e) override;

    /**
    * @brief Updates the dialogue UI.
    * 
    * @param deltaTime The delta time of the main game loop.
    */
    void Update(double deltaTime) override;

    /**
    * @brief Handles a command.
    * 
    * @param command Command to be handled.
    */
    void HandleCommand(const Command* command);

    /**
    * @brief Resizes the dialogue.
    *
    * @param widthScale The scale of the width (newWidth / oldWidth).
    * @param heightScale The scale of the height (newHeight / oldHeight).
    */
    void Resize(float widthScale, float heightScale) override;

    /**
    * @brief Sets the new state of the dialogue UI.
    * 
    * Great caution must be taken when this is used as it will change mid-state.
    *
    * @param newState The new state of the dialogue.
    */
    void SetState(DialogueState* newState);

    /**
    * @brief Schedules a state change. Will happen on the next update tick.
    * 
    * @param nextState The next state to be set.
    */
    void ScheduleStateChange(DialogueState* nextState);

    /**
    * @brief Returns the line label pool.
    * 
    * @return A reference to the line label pool.
    */
    UILabelPool& GetLineLabelPool();

    /**
    * @brief Sets the function to call on dialogue end.
    * 
    * The callback will be set on the current state, or as soon as a state if available.
    * 
    * \/!\ This callback will NOT persist after being set to a state. Subsequent states will NOT have this callback. /!\
    * 
    * @param callback Function to call on dialogue end.
    */
    void SetOnDialogueEnd(const std::function<void()>& callback);

    /**
    * @brief Sets the bounds of the dialogue box.
    *
    * @param newBounds New bounds of the dialogue box.
    */
    void SetBounds(const SDL_FRect& newBounds) override;

    /**
    * @brief Sets the background color of the dialogue box.
    *
    * @param newColor New background color.
    */
    void SetBackgroundColor(const SDL_Color& newColor) override;

    /**
    * @brief Sets the text color of the dialogue box.
    *
    * @param newColor New text color.
    */
    void SetTextColor(const SDL_Color& newColor) override;

    /**
    * @brief Sets the text font of the dialogue box.
    *
    * @param newFont New font of the text.
    */
    void SetTextFont(TTF_Font* newFont) override;

    /**
    * @brief Sets the text alignment of the dialogue box.
    *
    * @param newAlignment New alignment of the text.
    */
    void SetTextAlignment(const UITextAlignment& newAlignment) override;

    /**
    * @brief Sets the line spacing.
    * 
    * @param newSpacing New spacing for the lines.
    */
    void SetLineSpacing(float newSpacing);

    /**
    * @brief Returns the line spacing.
    * 
    * @return The line spacing of the dialogue.
    */
    float GetLineSpacing() const;

    /**
	* @brief Sets the typewriter delay between writing letters.
    * 
	* @param newDelay New typewriter delay in milliseconds.
    */
	void SetTypewriterDelay(float newDelay);

    /**
	* @brief Returns the typewriter delay between writing letters.
    * 
	* @return The typewriter delay in milliseconds.
    */
    float GetTypewriterDelay() const;

    /**
    * @brief Sets the padding (from the edges of the screen) of the dialogue.
    * 
    * @param newPadding The new padding of the dialogue.
    */
    void SetPadding(const SDL_FPoint& newPadding);

    /**
    * @brief Returns the padding (from the edges of the screen) of the dialogue.
    * 
    * @return The padding of the dialogue.
    */
    SDL_FPoint GetPadding() const;

    /**
    * @brief Sets the icon size of the arrows.
    * 
    * @param newSize New size of the icons.
    */
    void SetIconSize(float newSize);

    /**
    * @brief Returns the icon size of the arrows.
    * 
    * @return The icon size of the arrows.
    */
    float GetIconSize() const;

    /**
    * @brief Sets the arrow tile index for a certain direction.
    * 
    * @param tileName The name of the tile.
    * @param arrowDirection Direction of the arrow.
    */
    void SetArrowTileIndex(const std::string& tileName, Direction arrowDirection);

    /**
    * @brief Sets the arrow tile index for a certain direction.
    *
    * @param tileIndex The index of the tile.
    * @param arrowDirection Direction of the arrow.
    */
    void SetArrowTileIndex(int tileIndex, Direction arrowDirection);

    /**
    * @brief Sets the arrow transparency for a certain direction.
    *
    * @param alpha The transparency of the arrow.
    * @param arrowDirection Direction of the arrow.
    */
    void SetArrowTransparency(uint8_t alpha, Direction arrowDirection);

    /**
    * @brief Sets the arrow for a certain direction.
    *
    * @param arrow The arrow structure.
    * @param arrowDirection Direction of the arrow.
    */
    void SetArrow(DialogueArrow arrow, Direction arrowDirection);

    /**
    * @brief Returns the tile index of an arrow.
    * 
    * @param arrowDirection Direction of the arrow.
    * 
    * @return The arrow of the given direction.
    */
    DialogueArrow GetArrow(Direction arrowDirection) const;

    /**
    * @brief Checks if the UI dialogue states should show arrows or not.
    * 
    * @return 'SHOW_ARROWS / true' if they should, 'HIDE_ARROWS / false' if not.
    */
    ArrowVisibility ShouldShowArrows() const;

    /**
    * @brief Checks if the UI has a pending state to change to.
    * 
    * @return 'true' if the UI has a pending state to change to, 'false' otherwise.
    */
    bool HasPendingState() const;

    /**
    * @brief Checks if the UI currently has an active state.
    * 
    * @return 'true' if the UI has an active state, 'false' otherwise.
    */
    bool HasActiveState() const;

    /**
    * @brief Move operator.
    */
    UIDialogue& operator=(UIDialogue&& other) noexcept;
};

#endif // UIDIALOGUE_H_
