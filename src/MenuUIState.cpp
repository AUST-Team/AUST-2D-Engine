#include <SDL3/SDL.h>
#include <variant>

#include "MenuUIState.h"

#include "SystemActionRegistry.h"
#include "AppRenderSystem.h"
#include "GameConfiguration.h"
#include "UIPanel.h"
#include "MemoryTracker.h"
#include "Unused.h"
#include "Audio.h"

MenuUIState::MenuUIState(MenuUIState&& other) noexcept : 
    AppState(std::move(other)),
    currentPanel(std::exchange(other.currentPanel, nullptr)),
    panels(std::move(other.panels)),
    nextPanelKey(std::move(other.nextPanelKey)),
    pendingPanelChange(std::exchange(other.pendingPanelChange, false))
{
    other.panels.clear();
}

MenuUIState::~MenuUIState() { Cleanup(); }

void MenuUIState::HandleEvent(const SDL_Event& e)
{
    if (e.type == SDL_EVENT_WINDOW_RESIZED)
    {
        const GameConfiguration& config = Configuration::Get();

        const int newWidth = e.window.data1;
        const int newHeight = e.window.data2;
        const int baseWidth = config.window.width;
        const int baseHeight = config.window.height;

        ResizeMenu(newWidth, newHeight, baseWidth, baseHeight);

        AppRenderSystem::GetInstance().UpdateWindowSize();
    }

    if (currentPanel)
    {
        currentPanel->HandleEvent(e);
    }
}

void MenuUIState::Update(double deltaTime)
{
    if (pendingPanelChange)
    {
        auto it = panels.find(nextPanelKey);
        if (it != panels.end())
        {   
            if (it->second)
            {
                if (currentPanel)
                {
                    currentPanel->TriggerCallbacks(PanelCallbackType::PanelExit);
                }

                currentPanel = it->second;

                currentPanel->TriggerCallbacks(PanelCallbackType::PanelEnter);
            }
            else
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "MenuUIState.Update: Panel key [%s] exists, but panel entry or panel is NULL.", nextPanelKey.c_str());
            }
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "MenuUIState.Update: Attempted to switch to invalid panel key: '%s'", nextPanelKey.c_str());
        }
        pendingPanelChange = false;
    }

    if (currentPanel)
    {
        currentPanel->Update(deltaTime);
    }
}

void MenuUIState::Render(SDL_Renderer* renderer) const
{
    if (!SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "MenuUIState.Render: SetRendererDrawColor failed: %s", SDL_GetError());
    }

    if (!SDL_RenderClear(renderer))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "MenuUIState.Render: RenderClear failed: %s", SDL_GetError());
    }

    if (currentPanel)
    {
        currentPanel->Render(renderer);
    }

    if (!SDL_RenderPresent(renderer))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "MenuUIState.Render: RenderPresent failed: %s", SDL_GetError());
    }
}

void MenuUIState::Cleanup()
{
    for (auto& [_, panel] : panels)
    {
        ENG_DELETE(panel);
        panel = nullptr;
    }
    currentPanel = nullptr;
    panels.clear();
}

std::function<void(const SystemAction&)> MenuUIState::GetUIBinder()
{
    return [this](const SystemAction& action) {
        this->ProcessUIAction(action);
    };
}

MenuUIState& MenuUIState::operator=(MenuUIState&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    Cleanup();

	(*(AppState*)this) = std::move(other);

    currentPanel = std::exchange(other.currentPanel, nullptr);
    panels = std::move(other.panels);
    nextPanelKey = std::move(other.nextPanelKey);
    pendingPanelChange = std::exchange(other.pendingPanelChange, false);

    other.panels.clear();

    return *this;
}

void MenuUIState::SchedulePanelChange(const std::string& panelKey)
{
    nextPanelKey = panelKey;
    pendingPanelChange = true;
}

void MenuUIState::ProcessUIAction(const SystemAction& action)
{
    if (action.type == "changePanel")
    {

        if(action.payload.is_string())
        {
            SchedulePanelChange(action.payload.get_ref<const std::string&>());
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "MenuUIState.ProcessUIAction: Action type is 'changePanel', but payload is not a string.");
		}
    }
    else
    {
		SystemActionRegistry::GetInstance().Execute(action);
    }

    OnCustomUIAction(action);
}

void MenuUIState::CheckAndResizeMenu()
{
    const GameConfiguration& config = Configuration::Get();

    const int newWidth = AppRenderSystem::GetInstance().GetWidth();
    const int newHeight = AppRenderSystem::GetInstance().GetHeight();
    const int baseWidth = config.window.width;
    const int baseHeight = config.window.height;

    if (newWidth != baseWidth || newHeight != baseHeight)
    {
        ResizeMenu(newWidth, newHeight, baseWidth, baseHeight);
    }
}

void MenuUIState::ResizeMenu(int newWidth, int newHeight, int oldWidth, int oldHeight)
{
    const float widthScale = static_cast<float> (newWidth) / oldWidth;
    const float heightScale = static_cast<float> (newHeight) /  oldHeight;

    for (auto& [key, panelPtr] : panels)
    {
        if (panelPtr)
        {
            panelPtr->Resize(widthScale, heightScale);
            // Just to be extra safe.
            panelPtr->SetBounds(SDL_FRect{ 0.0f, 0.0f, static_cast<float>(newWidth), static_cast<float>(newHeight) });
        }
    }
}

void MenuUIState::OnCustomUIAction(const SystemAction& action) { UNUSED(action); }
