#include <SDL3/SDL.h>

#include "GameState.h"

#include "AppRenderSystem.h"
#include "KeyManager.h"
#include "Player.h"
#include "Camera.h"
#include "GameMap.h"
#include "MoveCommand.h"
#include "TogglePauseCommand.h"
#include "InteractCommand.h"
#include "Observer.h"
#include "Configuration.h"
#include "Guard.h"
#include "DialogueSystem.h"
#include "TileRegistry.h"
#include "GuardManager.h"
#include "SaveManager.h"
#include "FontManager.h"
#include "GameTime.h"
#include "AudioSystem.h"
#include "MemoryTracker.h"

GameState::GameState() :
    keyManager(KeyManager::GetInstance()),
    map(GameMap::GetInstance()),
    gameRenderer(&animationSystem)
{
    const int w = AppRenderSystem::GetInstance().GetWidth();
    const int h = AppRenderSystem::GetInstance().GetHeight();

    backgroundPanel.SetBackgroundColor(Configuration::Get().ui.background);
    backgroundPanel.SetBounds(SDL_FRect { 0.0f, 0.0f, static_cast<float> (w), static_cast<float> (h) });
}

GameState::~GameState() { Cleanup(); }

bool GameState::OnEnter()
{
    InitKeyManager();
    InitPlayer();
    InitCamera();
    InitMap();

    GameTime::StartNewTracking();

    if (SaveManager::HasPendingSave()) 
    {
        SaveManager::ApplyPendingSaveData();
    }

    return true;
}

void GameState::OnExit() { Cleanup(); }

void GameState::Cleanup()
{
    Player::DeleteInstance();
    Camera::DeleteInstance();
    map.Cleanup();
    keyManager.Cleanup();
    DialogueSystem::GetInstance().EndDialogue();
}

void GameState::HandleEvent(const SDL_Event& e)
{
    switch (e.type)
    {
        case SDL_EVENT_KEY_DOWN:
        {
            if (!e.key.repeat)
            {
                keyManager.OnKeyPressed(e.key.key);
            }
            break;
        }

        case SDL_EVENT_KEY_UP:
        {
            keyManager.OnKeyReleased(e.key.key);
            break;
        }

        case SDL_EVENT_MOUSE_WHEEL:
        {
            DialogueSystem::GetInstance().HandleEvent(e);
            break;
        }

        case SDL_EVENT_WINDOW_RESIZED:
        {
            const GameConfiguration& config = Configuration::Get();
            const int newWidth = e.window.data1;
            const int newHeight = e.window.data2;
            const int baseWidth = config.window.width;
            const int baseHeight = config.window.height;

            camera->SetWindowSize(newWidth, newHeight);

            backgroundPanel.SetBounds(SDL_FRect{ 0.0f, 0.0f, static_cast<float> (newWidth), static_cast<float> (newHeight) });

            DialogueSystem::GetInstance().ResizeUI(newWidth, newHeight, baseWidth, baseHeight);

            AppRenderSystem::GetInstance().UpdateWindowSize();

            break;
        }

        default:
        {
            break;
        }
    }
}

void GameState::Update(double deltaTime)
{
    player->Update(deltaTime);

    // Necessary for maps bigger than the viewport. Does nothing if the map is smaller.
    camera->Clamp(map.GetMapWidthPixels(), map.GetMapHeightPixels());

    GuardManager::GetInstance().Update(deltaTime);
    map.Update(deltaTime);
    animationSystem.Update(deltaTime);
    AudioSystem::GetInstance().Update(deltaTime);
}

void GameState::Render(SDL_Renderer* renderer) const
{
    if ( !SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255) )
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameState.Render: SetRendererDrawColor failed: %s", SDL_GetError());
    }

    if ( !SDL_RenderClear(renderer) )
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameState.Render: RenderClear failed: %s", SDL_GetError());
    }

    backgroundPanel.RenderPanel(renderer);

    const GameConfiguration& config = Configuration::Get();
    const int tileWidth = config.tiles.width;
    const int tileHeight = config.tiles.height;

    const int startX = std::max(0, static_cast<int>(camera->GetXOffset() / tileWidth));
    const int endX = std::min(map.GetMapWidth(), static_cast <int>(std::round((camera->GetXOffset() + camera->GetViewportWidth()) / tileWidth)) + 1);

    const int startY = std::max(0, static_cast<int>(camera->GetYOffset() / tileHeight));
    const int endY = std::min(map.GetMapHeight(), static_cast<int>(std::round((camera->GetYOffset() + camera->GetViewportHeight()) / tileHeight)) + 1);

    for (int y = startY; y < endY; ++y) 
    {
        for (int x = startX; x < endX; ++x) 
        {
            const std::vector<int>& cell = map.GetTiles(x, y);
            for (int idx : cell)
            {
                TileInstance* tile = TileRegistry::GetTileTypeByIndex(idx);
                if (tile)
                {
                    gameRenderer.SubmitTile(tile, x, y, camera);
                }
            }
        }
    }

    gameRenderer.SubmitPlayer(player, camera);

    for (Guard*& g : GuardManager::GetInstance().GetGuards())
    {
        gameRenderer.SubmitGuard(g, camera);
    }

    gameRenderer.Render(renderer);
    gameRenderer.Clear();

    DialogueSystem::GetInstance().Render(renderer);

    if ( !SDL_RenderPresent(renderer) )
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "GameState.Render: RenderPresent failed: %s", SDL_GetError());
    }
}

bool GameState::RenderUnderlyingStates() const { return AppState::RenderUnderlyingStates(); }

bool GameState::UpdateUnderlyingStates() const { return AppState::UpdateUnderlyingStates(); }

bool GameState::NotifyUnderlyingStates() const { return AppState::NotifyUnderlyingStates(); }

void GameState::InitKeyManager()
{
    if (keyManager.IsInitialised())
    {
        return;
    }

    keyManager.BindKey(SDLK_W, ENG_NEW(MoveCommand, Direction::Up));
    keyManager.BindKey(SDLK_A, ENG_NEW(MoveCommand, Direction::Left));
    keyManager.BindKey(SDLK_S, ENG_NEW(MoveCommand, Direction::Down));
    keyManager.BindKey(SDLK_D, ENG_NEW(MoveCommand, Direction::Right));

    keyManager.BindKey(SDLK_UP, ENG_NEW(MoveCommand, Direction::Up));
    keyManager.BindKey(SDLK_LEFT, ENG_NEW(MoveCommand, Direction::Left));
    keyManager.BindKey(SDLK_DOWN, ENG_NEW(MoveCommand, Direction::Down));
    keyManager.BindKey(SDLK_RIGHT, ENG_NEW(MoveCommand, Direction::Right));

    keyManager.BindKey(SDLK_ESCAPE, ENG_NEW(TogglePauseCommand));

    keyManager.BindKey(SDLK_E, ENG_NEW(InteractCommand));

    keyManager.SetInitialised(true);
}

void GameState::InitPlayer()
{
    player = Player::CreateInstance(0, 0);
}

void GameState::InitCamera()
{
    camera = Camera::CreateInstance(AppRenderSystem::GetInstance().GetWidth(), AppRenderSystem::GetInstance().GetHeight());

    player->AddObserver(camera);
}

void GameState::InitMap()
{
    map.LoadMapFromName(Configuration::Get().paths.startingMap);
    map.SetAnimationSystem(&animationSystem);
}