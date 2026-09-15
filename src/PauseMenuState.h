#pragma once

#ifndef PAUSEMENUSTATE_H_
#define PAUSEMENUSTATE_H_

#include "MenuUIState.h"

class PauseMenuState : public MenuUIState
{
private:
    UIPanel* mainPanelPtr = nullptr;    /// Pointer to the main (pause) panel. Non-owning.

    // No copying allowed due to possiblity of not being able to copy UIElements inside the UIPanel.
    PauseMenuState(const PauseMenuState&) = delete;
    PauseMenuState& operator=(const PauseMenuState&) = delete;

    /**
    * @brief Makes the pause menu.
    *
    * @return 'true' if the menu was created successfully, 'false' if otherwise.
    */
    bool MakeMenu();

public:

    /**
    * @brief Default constructor.
    */
    PauseMenuState() = default;

    /**
    * @brief Default destructor.
    */
    ~PauseMenuState() override = default;

    /**
    * @brief Default move constructor
    */
    PauseMenuState(PauseMenuState&& other) noexcept = default;

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
    * @return 'false' for the pause menu state.
    */
    bool RenderUnderlyingStates() const override;

    /*
    * @brief If the underlying states in the state stack should be updated or not.
    *
    * @return 'false' for the pause menu state.
    */
    bool UpdateUnderlyingStates() const override;

    /**
    * @brief If the underlying states in the state stack should be notified of events (and handle them) or not.
    *
    * @return 'true' for PauseMenuState
    */
    bool NotifyUnderlyingStates() const override;

    /**
    * @brief Default move operator.
    */
    PauseMenuState& operator=(PauseMenuState&& other) noexcept = default;
};

#endif