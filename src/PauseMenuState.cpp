#include <SDL3/SDL.h>

#include "PauseMenuState.h"

#include "UIFactory.h"
#include "UITextEffectSystem.h"

bool PauseMenuState::OnEnter()
{
    const bool success = MakeMenu();

    if (success)
    {
        CheckAndResizeMenu();
    }

    return success;
}

void PauseMenuState::OnExit() { Cleanup(); }

void PauseMenuState::Update(double deltaTime)
{
    MenuUIState::Update(deltaTime);

    if (currentPanel == mainPanelPtr)
    {
        UITextEffectSystem::GetInstance().Update(deltaTime);
    }
}

void PauseMenuState::Cleanup() 
{ 
    MenuUIState::Cleanup();
    UITextEffectSystem::GetInstance().Clear(); 
}

bool PauseMenuState::RenderUnderlyingStates() const { return AppState::RenderUnderlyingStates(); }

bool PauseMenuState::UpdateUnderlyingStates() const { return AppState::UpdateUnderlyingStates(); }

bool PauseMenuState::NotifyUnderlyingStates() const { return true; }

bool PauseMenuState::MakeMenu()
{
    mainPanelPtr = currentPanel = UIFactory::CreateUIByType(UIType::PauseMenu, panels, GetUIBinder());

    if (!currentPanel)
    {
        return false;
    }

    return true;
}