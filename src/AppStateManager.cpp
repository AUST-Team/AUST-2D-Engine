#include <SDL3/SDL.h>

#include "AppStateManager.h"

#include "Configuration.h"
#include "AppState.h"
#include "MainMenuState.h"
#include "PauseMenuState.h"
#include "AppRenderSystem.h"
#include "UITextEffectSystem.h"
#include "MemoryTracker.h"

void AppStateManager::Run()
{
    uint64_t now = 0;
    uint64_t last = SDL_GetPerformanceCounter();
    const double freq = static_cast<double>(SDL_GetPerformanceFrequency());
    const double frameDelay = Configuration::Get().app.frameDelayMs;
    double accumulator = 0.0;
    double frameTime = 0.0;

    running = true;
    while (running)
    {
        now = SDL_GetPerformanceCounter();
        frameTime = ((now - last) / freq) * 1000.0;
        last = now;

        if (frameTime > 250.0) 
        {
            frameTime = 250.0;
        }

        accumulator += frameTime;

        while (accumulator >= frameDelay)
        {
            if (!stateStack.empty())
            {
                HandleEvents();
                UpdateStates(frameDelay);
            }

            if (pendingQuit)
            {
                Quit();
            }

            if (!pendingStackActions.empty())
            {
                HandlePendingActions();
            }

            // Subtract frameDelay to preserve sub-millisecond remainders.
            accumulator -= frameDelay;
        }

        if (!stateStack.empty())
        {
            RenderStates(AppRenderSystem::GetInstance().GetRenderer());
        }

        if (frameDelay - accumulator > 2.0)
        {
            SDL_Delay(1);
        }
    }
}

void AppStateManager::Quit()
{
    Cleanup();
    running = false;
}

void AppStateManager::HandleEvents()
{
    AppRenderSystem::GetInstance().ProcessPendingWindowChanges();

    SDL_Event e;
    while (SDL_PollEvent(&e)) 
    {
        if (e.type == SDL_EVENT_QUIT)
        {
            ScheduleQuit();
        }

        if (!stateStack.empty())
        {
            HandleEventStates(e);
        }
    }
}

void AppStateManager::Cleanup()
{
    while (!stateStack.empty())
    {
        PopState();
    }
}

void AppStateManager::ChangeState(AppState* newState)
{
    if (!newState)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "AppStateManager.ChangeState: State to push is null.");
        return;
    }

    while (!stateStack.empty())
    {
        PopState();
    }

    UITextEffectSystem::GetInstance().Clear();

    PushState(newState);
}

void AppStateManager::HandlePendingActions()
{
    for (const StackAction& action : pendingStackActions) 
    {
        switch (action.type)
        {
            case StackActionType::Push:
            {
                PushState(action.state);
                break;
            }

            case StackActionType::Pop:
            {
                PopState();
                break;
            }

            case StackActionType::Change:
            {
                ChangeState(action.state);
                break;
            }

            default:
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "AppStateManager.HandlePendingActions: Encountered unknown stack action type.");
                break;
			}
        }
    }
    pendingStackActions.clear();

    if (stateStack.empty())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "AppStateManager.HandlePendingActions: State stack empty after processing all pending actions, defaulting to main menu.");
        ChangeState(ENG_NEW(MainMenuState));
    }
}

void AppStateManager::UpdateStates(double deltaTime)
{
    for (auto it = stateStack.rbegin(); it != stateStack.rend(); ++it) 
    {
        (*it)->Update(deltaTime);
        // If this state has this method returning false, then we stop going deeper.
        if ( !(*it)->UpdateUnderlyingStates() )
        {
            break;
        }
    }
}

void AppStateManager::RenderStates(SDL_Renderer* renderer) const
{
    for (auto it = stateStack.rbegin(); it != stateStack.rend(); ++it) 
    {
        (*it)->Render(renderer);
        // If this state has this method returning false, then we stop going deeper.
        if ( !(*it)->RenderUnderlyingStates() )
        {
            break;
        }
    }
}

void AppStateManager::HandleEventStates(const SDL_Event& e) const
{
    for (auto it = stateStack.rbegin(); it != stateStack.rend(); ++it)
    {
        (*it)->HandleEvent(e);
        // If this state has this method returning false, then we stop going deeper.
        if (!(*it)->NotifyUnderlyingStates())
        {
            break;
        }
    }
}

void AppStateManager::ScheduleStateChange(AppState* newState)
{ 
    pendingStackActions.push_back(StackAction { .type = StackActionType::Change, .state = newState });
}

void AppStateManager::ScheduleQuit() { pendingQuit = true; }

void AppStateManager::SchedulePop() 
{ 
    pendingStackActions.push_back(StackAction { .type = StackActionType::Pop, .state = nullptr });
}

void AppStateManager::SchedulePushState(AppState* state)
{
    pendingStackActions.push_back(StackAction { .type = StackActionType::Push, .state = state });
}

bool AppStateManager::IsCurrentStatePause() const
{
    return !stateStack.empty() &&
        dynamic_cast<PauseMenuState *>(stateStack.back()) != nullptr;
}

void AppStateManager::PushState(AppState* newState)
{
    if (!newState)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "AppStateManager.PushState: State to push is null.");
        return;
    }

    if (!newState->OnEnter())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AppStateManager.PushState: OnEnter failed for new state.");
        ENG_DELETE(newState);
        return;
    }

    stateStack.push_back(newState);
}

void AppStateManager::PopState()
{
    if (!stateStack.empty() && stateStack.back())
    {
        stateStack.back()->OnExit();
        stateStack.back()->Cleanup();
        ENG_DELETE(stateStack.back());
        stateStack.pop_back();
    }
}
