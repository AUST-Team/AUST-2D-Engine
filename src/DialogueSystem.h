#pragma once

#ifndef DIALOGUESYSTEM_H_
#define DIALOGUESYSTEM_H_

#include <functional>

#include "UIDialogue.h"

struct SDL_Renderer;
class Command;

/**
* @brief Dialogue system. Acts like a container, singleton and proxy for the an UIDialogue element.
*/
class DialogueSystem 
{
private:
    UIDialogue* ui = nullptr;   /// UI for the dialogue. Owned.
    bool isActiveFlag = false;  /// Flag if the dialogue system is currently active.

    /**
    * @brief Constructor.
    */
    DialogueSystem();

    /**
    * @brief Destructor.
    */
    ~DialogueSystem();

    // No copying or moving allowed due to singleton pattern.
    DialogueSystem(const DialogueSystem&) = delete;
    DialogueSystem& operator=(const DialogueSystem&) = delete;
    DialogueSystem(DialogueSystem&&) = delete;
    DialogueSystem& operator=(DialogueSystem&&) = delete;

public:

    /**
    * @brief Returns or creates an instance of the dialogue system.
    * 
    * @return Reference to the dialogue system instance.
    */
    static DialogueSystem& GetInstance() noexcept
    {
        static DialogueSystem instance; // The default constructor is RIGHT there, Intellisense.
        return instance;
    }

    /**
    * @brief Handles an event.
    * 
    * @param e Event to be handled.
    */
    void HandleEvent(const SDL_Event& e);

    /**
    * @brief Updates the dialogue system.
    * 
    * @param deltaTime The deltaTime of the main SDL loop.
    */
    void Update(double deltaTime);

    /**
    * @brief Renders the dialogue UI.
    * 
    * @param renderer Pointer to the SDL Renderer used.
    */
    void Render(SDL_Renderer* renderer) const;

    /**
    * @brief Handles a command.
    *
    * @param command Command to be handled.
    */
    void HandleCommand(const Command* command);

    /**
    * @brief Starts the dialogue.
    * 
    * @param newlines Vector of lines to be displayed.
    * @param callback Function call on dialogue end.
    */
    void StartDialogue(const std::vector<std::string>& newLines, std::function<void()> callback = nullptr);

    /**
    * @brief Starts a choice selection.
    * 
    * @param choices The choices that can be selected.
    * @param callbacks The functions (per choice) to be called once a choice is selected.
    * @param question The question to be asked before making a choice.
    * @param onEndCallback Function to call once the dialogue ends.
    */
    void StartChoice(
        const std::vector<std::string>& choices, 
        const std::vector<std::function<void()>>& callbacks = {}, 
        const std::string& question = nullptr,
		const std::function<void()>& onEndCallback = nullptr
    );

    /**
    * @brief Ends the dialogue, clears the UI and sets the dialogue system to 'inactive'.
    */
    void EndDialogue();

    /**
    * @brief Clears the text without closing the dialogue system.
    */
    void Clear();

    /**
    * @brief Resizes the UI relative to the previous window size.
    *
    * @param newWidth New width of the window.
    * @param newHeight New height of the window.
    * @param oldWidth Old width of the window.
    * @param oldHeight Old height of the window.
    */
    void ResizeUI(int newWidth, int newHeight, int oldWidth, int oldHeight);

    /**
    * @brief Sets the function to call on dialogue end.
    * 
    * @param callback The function to call on dialogue end.
    */
    void SetOnDialogueEnd(std::function<void()> callback);

    /**
    * @brief Sets the dialogue system active or inactive.
    * 
    * @param newValue New value for the dialogue system's active status.
    */
    void SetActive(bool newValue);

    /**
    * @brief Returns if the system is active.
    * 
    * @return 'true' if the dialogue system is active, false if otherwise.
    */
    bool IsActive() const;
};

#endif // DIALOGUESYSTEM_H_
