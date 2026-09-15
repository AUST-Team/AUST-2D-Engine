#pragma once

#ifndef DIALOGUECHOICESTATE_H_
#define DIALOGUECHOICESTATE_H_

#include <string>

#include "DialogueState.h"
#include "UILabelPool.h"
#include "UITileImage.h"

/**
* @brief The state for a dialogue where the player can choose from multiple choices.
*/
class DialogueChoiceState : public DialogueState
{
private:
    UIDialogue& dialogue;   /// Reference to the dialogue UI.

    std::string question;               /// The question line.
    std::vector<std::string> choices;   /// The choices.
    std::vector<std::function<void()>> choiceCallbacks;   /// Functions (per choice) to call once a choice has been selected.

    mutable std::vector<std::vector<std::string>> wrappedChoiceLines;   /// The wrapped lines of the choices.
    mutable std::vector<std::string> wrappedQuestionLines;              /// The wrapped lines of the question.

    // The question uses the dialogue line label pool.
    mutable std::vector<UILabelPool> choiceLabelPools;  /// Label pool for the choices.

    UITileImage upArrowIcon;    /// Image for the up arrow icon.
    UITileImage downArrowIcon;  /// Image for the down arrow icon.

    int selectedIndex = 0;     /// Selected choice index.
    int scrollOffset = 0;      /// The scroll offset.
    int maxVisibleLines = 0;   /// Maximum visible lines.

    bool questionTypingFlag = true;     /// Flag if currently typing the question.
    bool choicesTypingFlag = true;      /// Flag if currently typing the choices.
    bool finishTypingFlag = false;      /// Flag if finished typing.

    static constexpr const char* questionEffectID = "__dialogue_question";    /// The ID of the choices.
	static constexpr const char* choiceEffectIDPrefix = "__dialogue_choice_";    /// The prefix for the ID of the choices.

    /**
    * @brief Updates the question label pool.
    */
    void UpdateQuestionLabelPool();

    /**
    * @brief Updates the choice label pool.
    */
    void UpdateChoiceLabelPool();

    /**
    * @brief Updates the choice label highlighting.
    */
    void UpdateChoiceHighlighting();

    /**
    * @brief Updates the choice label positions.
    */
    void UpdateChoiceLabelPositions();

    /**
    * @brief Updates the scroll icons.
    */
    void UpdateScrollIcons();

    /**
    * @brief Adds a choice to the label pool.
    * 
    * @param choice The full text of the choice.
    * @param index Index of the choice.
    */
    void AddChoiceToPool(std::string choice, size_t index);

    /**
    * @brief Scrolls up the text.
    */
    void ScrollUp();

    /**
    * @brief Scrolls down the text.
    */
    void ScrollDown();

    /**
    * @brief Selects the previous choice.
    */
    void SelectPreviousChoice();

    /**
    * @brief Selects the next choice.
    */
    void SelectNextChoice();

    /**
    * @brief Skips typing the text, fully displays it instantly.
    */
    void SkipTyping();

    /**
    * @brief Confirms the selected choice.
    */
    void ConfirmChoice();

    /**
    * @brief Recalculates the maximum visible lines.
    * 
    * @param padding The vertical padding of the dialogue box.
    */
    void RecalculateMaxVisibleLines(float padding);

public:

    /**
    * @brief Constructor.
    * 
    * @param dialogue Back reference to the dialogue UI.
    * @param question Question to ask.
    * @param choices Vector of choices to select from.
    * @param callbacks Functions (per choice) to call once a choice is selected.
	* @param onDialogueEnd Function to call at the end of dialogue.
    */
    DialogueChoiceState(
        UIDialogue& dialogue,
        const std::string& question = " ",
        const std::vector<std::string>& choices = {},
        const std::vector<std::function<void()>>& callbacks = {},
		const std::function<void()>& onDialogueEnd = nullptr
    );

    /**
    * @brief Enters the dialogue choice state.
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
    * @brief Updates the choice state.
    * 
    * @param deltaTime The delta time of the main app loop.
    */
    void Update(double deltaTime) override;

    /**
    * @brief Renders the choice state.
    * 
    * @param renderer Pointer to the SDL_Renderer used.
    */
    void Render(SDL_Renderer* renderer) const override;

    /**
    * @brief Exist the state.
    *
    * @param dialogueRef Reference to the dialogue UI.
    */
    void Exit(UIDialogue& dialogueRef) override;

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
    * @brief Refreshes the dialogue choice state.
    */
    void RefreshState() override;

    /**
    * @brief Checks if the state is shareable or not.
    *
    * @return 'false', as the state is not shareable.
    */
    bool IsShared() const override;
};

#endif // DIALOGUECHOICESTATE_H_