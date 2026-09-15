#pragma once

#ifndef DIALOGUENARRATIONSTATE_H_
#define DIALOGUENARRATIONSTATE_H_

#include <string>
#include <functional>
#include <vector>

#include "DialogueState.h"
#include "UITileImage.h"

/**
* @brief Dialogue state for the default dialogue state.
* 
* I have no better name for this, but just that it renders lines of dialogue.
*/
class DialogueNarrationState : public DialogueState 
{
private:
    UIDialogue& dialogue;         /// Reference to the dialogue UI.

    std::vector<std::string> dialogueLines;     /// Vector for all the lines of text.
    size_t currentLineIndex = 0;    /// Index of the current dialogue line.
    std::vector<std::string> wrappedLines;      /// Vector of wrapped lines for the current line of text.

    int scrollOffset = 0;      /// Scroll offset.
    int maxVisibleLines = 0;   /// Maximum number of visible lines.

    UITileImage upArrowIcon;    /// Image for the up arrow icon.
    UITileImage downArrowIcon;  /// Image for the down arrow icon.

    static constexpr const char* effectID = "__dialogue_line";    /// The ID of the typewriter effect.

    // No assignment operators allowed due to references.
    DialogueNarrationState& operator=(const DialogueNarrationState&) = delete;
    DialogueNarrationState& operator=(DialogueNarrationState&&) = delete;

    // No copying due to cached textures in UITileImage.
    DialogueNarrationState(const DialogueNarrationState&) = delete;

    /**
    * @brief Updates the line label pool.
    */
    void UpdateLineLabelPool();

    /**
    * @brief Updates the scroll icons.
    */
    void UpdateScrollIcons();

    /**
    * @brief Starts the current line.
    */
    void StartCurrentLine();

    /**
    * @brief Scrolls up the dialogue.
    */
    void ScrollUp();

    /**
    * @brief Scrolls down the dialogue.
    */
    void ScrollDown();

    /**
    * @brief Skips the dialogue typing.
    */
    void SkipTyping();

    /**
    * @brief Goes to the next line.
    */
    void NextLine();

    /**
    * @brief Recalculates the maximum visible lines.
    * 
    * @param padding Vertical padding.
    */
    void RecalculateMaxVisibleLines(float padding);

public:

    /**
    * @brief Constructor.
    * 
    * @param onFinished Function to call at the end of dialogue.
    * @param dialogue Reference to the UI dialogue used.
    * @param lines The lines of dialogue.
    */
    DialogueNarrationState(
        UIDialogue& dialogue, 
        const std::vector<std::string>& lines = {},
        const std::function<void()>& onFinished = nullptr
    );

    /**
    * @brief Default move constructor.
    */
    DialogueNarrationState(DialogueNarrationState&& other) noexcept = default;

    /**
    * @brief Default destructor.
    */
    ~DialogueNarrationState() override = default;

    /**
    * @brief Enters the dialogue state.
    * 
    * @param dialogueRef Reference to the dialogue UI.
    */
    void Enter(UIDialogue& dialogueRef) override;

    /**
    * @brief Handles an event.
    * 
    * @param e Event to be handled.
    */
    void HandleEvent(const SDL_Event& e) override;

    /**
    * @brief Updates the dialogue state.
    * 
    * @param deltaTime The delta time of the main app loop.
    */
    void Update(double deltaTime) override;

    /**
    * @brief Renders the dialogue state.
    * 
    * @pararm renderer Pointer to the SDL Renderer used.
    */
    void Render(SDL_Renderer* renderer) const override;

    /**
    * @brief Handles a command.
    * 
    * @param command Command to be handled.
    */
    void HandleCommand(const Command* command) override;

    /**
    * @brief Resizes the dialogue state.
    *
    * @param widthScale The scale of the width (newWidth / oldWidth).
    * @param heightScale The scale of the height (newHeight / oldHeight).
    */
    void Resize(float widthScale, float heightScale) override;

    /**
    * @brief Exist the dialogue state.
    * 
    * @param dialogueRef Reference to the dialogue UI.
    */
    void Exit(UIDialogue& dialogueRef) override;

    /**
    * @brief Refreshes the dialogue state.
    */
    void RefreshState() override;

    /**
    * @brief Checks if the state is shareable or not.
    * 
    * @return 'false', as the state is not shareable.
    */
    bool IsShared() const override;
};

#endif // DIALOGUENARRATIONSTATE_H_
