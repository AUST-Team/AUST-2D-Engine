#include <SDL3/SDL.h>

#include "CutsceneState.h"

#include "AppRenderSystem.h"
#include "Configuration.h"
#include "FontManager.h"
#include "UILabel.h"
#include "AppStateManager.h"
#include "GameState.h"
#include "MainMenuState.h"
#include "UIButton.h"
#include "UITextField.h"    
#include "Player.h"
#include "ScoreManager.h"
#include "GameTime.h"
#include "Miscs.h"
#include "UICutsceneTextWriter.h"
#include "MemoryTracker.h"
#include "UIFactory.h"

CutsceneState::CutsceneState(const std::string& cutsceneName) :
    cutsceneName(cutsceneName) {}

bool CutsceneState::OnEnter()
{
    const bool success = MakeCutscene();

    if (success)
    {
        CheckAndResizeMenu();
    }

    return success;
}

void CutsceneState::OnExit() { Cleanup(); }

void CutsceneState::Update(double deltaTime)
{
    if (showNameInputFlag)
    {
        //MakeNameInput();
        showNameInputFlag = false;
    }

    MenuUIState::Update(deltaTime);
}

bool CutsceneState::RenderUnderlyingStates() const { return AppState::RenderUnderlyingStates(); }

bool CutsceneState::UpdateUnderlyingStates() const { return AppState::UpdateUnderlyingStates(); }

bool CutsceneState::NotifyUnderlyingStates() const { return AppState::NotifyUnderlyingStates(); }

bool CutsceneState::MakeCutscene()
{
    currentPanel = UIFactory::CreateCutsceneFromName(cutsceneName, panels, GetUIBinder());

    if (!currentPanel)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "CutsceneState.MakeCutscene: Failed to create cutscene from UI factory.");
        return false;
    }

    return true;
}
/*
void CutsceneState::MakeNameInput()
{
    panel.Clear();

    auto& config = Configuration::Get();
    auto& fontPath = config.paths.fontFilePath;
    unsigned int screenWidth = config.window.width;
    unsigned int screenHeight = config.window.height;
    float hintFontSize = config.ui.fonts.hint;

    TTF_Font* textFont = FontManager::GetInstance().GetFontByPath(fontPath, hintFontSize);
    if (!textFont)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "CutsceneState.MakeNameInput: Failed to load font: %s", SDL_GetError());
        return;
    }

    float elementWidth = 320.0f;
    float elementHeight = 40.0f;

    float centerX = (screenWidth - elementWidth) / 2.0f;
    float baseY = (screenHeight - (elementHeight * 3 + 30)) / 2.0f;

    auto label = ENG_NEW(UILabel, 
        SDL_FRect{ centerX, baseY, elementWidth, elementHeight },
        SDL_Color{ 0, 0, 0, 0 },
        SDL_Color{ 255, 255, 255, 255 },
        "Insert Name:",
        textFont
    );

    auto nameField = ENG_NEW(UITextField, 
        SDL_FRect{ centerX, baseY + elementHeight + 10.0f, elementWidth, elementHeight },
        SDL_Color{ 30, 30, 30, 255 },
        SDL_Color{ 255, 255, 255, 255 },
        textFont,
        SDL_Color{ 255, 255, 255, 255 },
        UITextAlignment{ HorizontalAlignment::Left, VerticalAlignment::Middle },
        "Name (leave blank to skip)"
    );

    auto submitButton = ENG_NEW(UIButton, 
        SDL_FRect{ centerX + elementWidth / 4, baseY + (elementHeight + 10.0f) * 2 + 10.0f, elementWidth / 2, elementHeight },
        SDL_Color{ 0, 0, 0, 0 },
        SDL_Color{ 255, 255, 255, 255 },
        "Submit",
        textFont
    );

    submitButton->SetOnClick([nameField]() {
        std::string name = nameField->GetText();
        if (!name.empty()) 
        {
            unsigned int timesCaught = Player::GetInstance()->GetTimesCaught();
            uint64_t playTime = GameTime::GetPlayTimeSeconds();
            ScoreManager scoreManager;
            scoreManager.LoadScoresFromFile();
            scoreManager.AddScore(name, timesCaught, playTime);
        }

        AppStateManager::GetInstance().ScheduleStateChange(ENG_NEW(MainMenuState));
    });

    StylizeButton(submitButton);

    panel.AddElement(label);
    panel.AddElement(nameField);
    panel.AddElement(submitButton);
}

void CutsceneState::SetOnCutsceneComplete(UICutsceneTextWriter* cutsceneWriter)
{
    switch (type)
    {
        case CutsceneType::Prologue:
        {
            cutsceneWriter->SetOnComplete([this]()
                {
                    AppStateManager::GetInstance().ScheduleStateChange(ENG_NEW(GameState));
                    GameTime::StartNewTracking();
                    //showNameInputFlag = true;
                }
            );
            break;
        }

        case CutsceneType::EpilogueBAD:
        {
            cutsceneWriter->SetOnComplete([]()
                {
                    AppStateManager::GetInstance().ScheduleStateChange(ENG_NEW(MainMenuState));
                }
            );
            break;
        }

        case CutsceneType::EpiloguePC:
        case CutsceneType::EpilogueMDF:
        {
            cutsceneWriter->SetOnComplete([this]()
                {
                    showNameInputFlag = true;
                }
            );
            break;
        }

        default:
        {
            cutsceneWriter->SetOnComplete([this]()
                {
                    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "CutsceneState.SetOnCutsceneComplete: Unknown cutscene type. Defaulting to main menu.");
                    AppStateManager::GetInstance().ScheduleStateChange(ENG_NEW(MainMenuState));
                }
            );
            break;
        }
    }
}
*/
