#pragma once

#ifndef CUTSCENESTATE_H_
#define CUTSCENESTATE_H_

#include "MenuUIState.h"
#include "UIPanel.h"

class UICutsceneTextWriter;

class CutsceneState : public MenuUIState
{
private:
    std::string cutsceneName;   /// Cutscene name.
    bool showNameInputFlag = false; /// Flag if the name score input should be shown.

    // No copying allowed due to possiblity of not being able to copy UIElements inside the UIPanel.
    CutsceneState(const CutsceneState&) = delete;
    CutsceneState& operator=(const CutsceneState&) = delete;

    /**
    * @brief Creates the cutscene.
    */
    bool MakeCutscene();

public:

    /**
    * @brief Constructor. Creates the panel.
    * 
    * @param cutsceneName The name of the cutscene.
    */
    CutsceneState(const std::string& cutsceneName);

    /**
    * @brief Default destructor.
    */
    ~CutsceneState() override = default;

    /**
    * @brief Default move constructor
    */
    CutsceneState(CutsceneState&& other) noexcept = default;

    /**
    * @brief Defines what happens when the custscene state is entered.
    *
    * @return true if the enter was successfull, false otherwise (error).
    */
    bool OnEnter() override;

    /**
    * @brief Defines what happens when the custscene state is exited.
    */
    void OnExit() override;

    /**
    * @brief Updates the menu.
    *
    * @param deltaTime The deltaTime of the SDL loop.
    */
    void Update(double deltaTime) override;

    /*
    * @brief If the underlying states in the state stack should be rendered or not.
    *
    * @return 'false' for the cutscene state.
    */
    bool RenderUnderlyingStates() const override;

    /*
    * @brief If the underlying states in the state stack should be updated or not.
    *
    * @return 'false' for the cutscene state.
    */
    bool UpdateUnderlyingStates() const override;

    /**
    * @brief If the underlying states in the state stack should be notified of events (and handle them) or not.
    *
    * @return 'false' for the cutscene state.
    */
    bool NotifyUnderlyingStates() const override;

    /**
    * @brief Default move operator.
    */
    CutsceneState& operator=(CutsceneState&& other) noexcept = default;
};

#endif // CUTSCENESTATE_H_