#pragma once

#ifndef DIALOGUESTATE_H_
#define DIALOGUESTATE_H_

#include <functional>

struct SDL_Renderer;
union SDL_Event;
class UIDialogue;
class Command;

/**
* @brief Abstract class for a dialogue state.
*/
class DialogueState 
{
protected:
	std::function<void()> onDialogueEndCallback = nullptr;  /// Function to call on dialogue end.

public:

    /**
    * @brief Constructor.
    * 
	* @param onDialogueEnd Function to call on dialogue end.
    */
	explicit DialogueState(const std::function<void()>& onDialogueEnd = nullptr);

    /**
    * @brief Default destructor.
    */
    virtual ~DialogueState() = default;

    /**
    * @brief Enters the dialogue state.
    * 
    * @param dialogueRef Reference to the dialogue UI.
    */
    virtual void Enter(UIDialogue& dialogueRef) = 0;

    /**
    * @brief Handles an event.
    * 
    * @param e Reference to the event to be handled.
    */
    virtual void HandleEvent(const SDL_Event& e) = 0;

    /**
    * @brief Updates the dialogue state.
    * 
    * @param deltaTime The delta time of the main app loop.
    */
    virtual void Update(double deltaTime) = 0;

    /**
    * @Brief Renders the dialogue state.
    * 
    * @param renderer Pointer to the SDL_Renderer used.
    */
    virtual void Render(SDL_Renderer* renderer) const = 0;

    /**
    * @brief Exist the state.
    * 
    * @param dialogueRef Reference to the dialogue UI.
    */
    virtual void Exit(UIDialogue& dialogueRef) = 0;

    /**
    * @brief Handles a command.
    * 
    * @param command Command to be handled.
    */
    virtual void HandleCommand(const Command* command) = 0;

    /**
    * @brief Resizes the dialogue state.
    *
    * @param widthScale The scale of the width (newWidth / oldWidth).
    * @param heightScale The scale of the height (newHeight / oldHeight).
    */
    virtual void Resize(float widthScale, float heightScale) = 0;

    /**
    * @brief Refreshes the dialogue state, if necessary.
    */
    virtual void RefreshState() = 0;

    /**
    * @brief Sets the function to call on dialogue end.
    * 
    * @param callback Function to call on dialogue end.
    */
    virtual void SetOnDialogueEnd(const std::function<void()>& callback);

    /**
    * @brief Checks if the state is shareable.
    * 
    * @return 'true' if the state is shareable, 'false' if otherwise.
    */
    virtual bool IsShared() const = 0;
};

#endif // DIALOGUESTATE_H_
