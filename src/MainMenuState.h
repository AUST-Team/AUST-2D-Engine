#pragma once

#ifndef MAINMENUSTATE_H_
#define MAINMENUSTATE_H_

#include "MenuUIState.h"

/**
* @brief Encapsulates the main menu GUI.
*/
class MainMenuState : public MenuUIState 
{
private:
    UIPanel* mainPanelPtr = nullptr;    /// Pointer to the main (title) panel. Non-owning.
    static bool firstUpdateFlag;    /// Flag if this is the first time the MainMenuState gets created.

    // No copying allowed due to possiblity of not being able to copy UIElements inside the UIPanel.
    MainMenuState(const MainMenuState&) = delete;
    MainMenuState& operator=(const MainMenuState&) = delete;

    /**
    * @brief Makes the main menu GUI
    *
    * @return 'true' if the menu was created successfully, 'false' if otherwise.
    */
    bool MakeMenu();

public:

    /**
    * @brief Default constructor.
    */
    MainMenuState() = default;

    /**
    * @brief Default destructor.
    */
    ~MainMenuState() override = default;

    /**
    * @brief Default move constructor
    */
    MainMenuState(MainMenuState&& other) noexcept = default;

    /**
    * @brief Defines what happens when the main menu state is entered.
    *
    * @return true if the enter was successfull, false otherwise (error).
    */
    bool OnEnter() override;

    /**
    * @brief Defines what happens when the main menu state is exited.
    */
    void OnExit() override;

    /**
    * @brief Updates the menu.
    * 
    * @param deltaTime The deltaTime of the SDL loop.
    */
    void Update(double deltaTime) override;

    /**
    * @brief Cleans up resources.
    */
    void Cleanup() override;

    /*
    * @brief If the underlying states in the state stack should be rendered or not.
    *
    * @return 'false' for the main menu state.
    */
    bool RenderUnderlyingStates() const override;

    /*
    * @brief If the underlying states in the state stack should be updated or not.
    *
    * @return 'false' for the main menu state.
    */
    bool UpdateUnderlyingStates() const override;

    /**
    * @brief If the underlying states in the state stack should be notified of events (and handle them) or not.
    *
    * @return 'false' for the main menu state.
    */
    bool NotifyUnderlyingStates() const override;

    /**
    * @brief Default move operator.
    */
    MainMenuState& operator=(MainMenuState&& other) noexcept = default;
};

#endif // MAINMENUSTATE_H_