#pragma once

#ifndef MENUUISTATE_H_
#define MENUUISTATE_H_

#include <unordered_map>
#include <string>
#include <functional>

#include "AppState.h"

class UIPanel;
struct SystemAction;

/**
* @brief Abstract UI state for any states that have a UI or are primarily UI.
*/
class MenuUIState : public AppState 
{
protected:
	UIPanel* currentPanel = nullptr;                     /// Pointer to the currently active panel. Non-allocated.
    std::unordered_map<std::string, UIPanel*> panels;    /// Map of panels, associated with a string (name of the panel). OWNING!
	std::string nextPanelKey;           /// Key of the next panel to switch to. Empty if no panel change is scheduled.
	bool pendingPanelChange = false;    /// Flag if a panel change is scheduled for the next frame update.

private:

    // No copying allowed due to possiblity of not being able to copy UIElements inside the UIPanel.
    MenuUIState(const MenuUIState&) = delete;
    MenuUIState& operator=(const MenuUIState&) = delete;

    /**
	* @brief Schedules a panel change for the next frame update.
    * 
	* Will be overwritten if called multiple times before the next frame update.
    * 
	* @param panelKey The key of the panel to switch to.
    */
    virtual void SchedulePanelChange(const std::string& panelKey);

    /**
	* @brief Processes a UI action.
    * 
	* @param action The UI action to process.
    */
    virtual void ProcessUIAction(const SystemAction& action);

protected: 

    /**
    * @brief Checks if the menu needs to be resized, and if so, resizes it.
    * 
    * This is mainly used when first making the menu to see if it's different from the base window size.
    */
    virtual void CheckAndResizeMenu();

    /**
    * @brief Resizes the menu relative to the previous window size.
    *
    * @param newWidth New width of the window.
    * @param newHeight New height of the window.
    * @param oldWidth Old width of the window.
    * @param oldHeight Old height of the window.
    */
    virtual void ResizeMenu(int newWidth, int newHeight, int oldWidth, int oldHeight);

    /**
	* @brief Hook for inherited states to implement specific behavior for custom UI actions.
    * 
	* @param action The UI action to process.
    */
    virtual void OnCustomUIAction(const SystemAction& action);

public:

    /**
    * @brief Default constructor.
    */
    MenuUIState() = default;

    /**
    * Move constructor.
    */
    MenuUIState(MenuUIState&& other) noexcept;

    /**
    * @brief Destructor.
    */
    virtual ~MenuUIState();

    /**
    * @brief Handles an event.
    *
    * @param e The event to be handled.
    */
    virtual void HandleEvent(const SDL_Event& e) override;

    /**
	* @brief Updates the current panel and processes any scheduled panel changes.
    * 
    * @param deltaTime The delta time from the main game loop.
    */
    virtual void Update(double deltaTime) override;

    /**
    * @brief Renders the current state.
    *
    * @param renderer Pointer to the SDL Renderer used.
    */
    virtual void Render(SDL_Renderer* renderer) const override;

    /**
    * @brief Cleans up any resources used by the state.
    */
    virtual void Cleanup() override;

    /**
	* @brief Returns a function that can be used to bind UI actions to the processing method.
    * 
	* @return A function that takes an UI action and processes it.
    */
    virtual std::function<void(const SystemAction&)> GetUIBinder();

    /**
    * Move operator.
    */
    MenuUIState& operator=(MenuUIState&& other) noexcept;
};

#endif // MENUUISTATE_H_