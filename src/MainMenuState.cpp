#include "MainMenuState.h"

#include "AudioSystem.h"
#include "UITextEffectSystem.h"
#include "UIFactory.h"
#include "UIPanel.h"

bool MainMenuState::firstUpdateFlag = false;

bool MainMenuState::OnEnter() 
{
    const bool success = MakeMenu();

    if (!firstUpdateFlag)
    {
        firstUpdateFlag = true;
        return success;
    }

    if (success)
    {
        CheckAndResizeMenu();
    }

    return success;
}

void MainMenuState::OnExit() { Cleanup(); }

void MainMenuState::Update(double deltaTime) 
{
    MenuUIState::Update(deltaTime);
    AudioSystem::GetInstance().Update(deltaTime);

    if (currentPanel == mainPanelPtr)
    {
        UITextEffectSystem::GetInstance().Update(deltaTime);
    }
}

void MainMenuState::Cleanup() 
{
    MenuUIState::Cleanup();
    UITextEffectSystem::GetInstance().Clear(); 
}

bool MainMenuState::RenderUnderlyingStates() const { return AppState::RenderUnderlyingStates(); }

bool MainMenuState::UpdateUnderlyingStates() const { return AppState::UpdateUnderlyingStates(); }

bool MainMenuState::NotifyUnderlyingStates() const { return AppState::NotifyUnderlyingStates(); }
 
bool MainMenuState::MakeMenu()
{
    mainPanelPtr = currentPanel = UIFactory::CreateUIByType(UIType::MainMenu, panels, GetUIBinder());

    if (!currentPanel)
    {
        return false;
    }

    currentPanel->TriggerCallbacks(PanelCallbackType::PanelEnter);

    return true;
}