#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Application.h"

#include "AppRenderSystem.h"
#include "AppStateManager.h"
#include "MainMenuState.h"
#include "FontManager.h"
#include "Assets.h"
#include "TileRegistry.h"
#include "AudioSystem.h"
#include "Configuration.h"
#include "MemoryTracker.h"
#include "IOConfiguration.h"
#include "ModManager.h"
#include "Logging.h"
#include "FlagManager.h"
#include "DataProvider.h"
#include "GameTime.h"
#include "ScoreManager.h"
#include "SettingsManager.h"
#include "SystemValueRegistry.h"
#include "Audio.h"
#include "ConstantConfiguration.h"
#include "SystemActionRegistry.h"
#include "GameActionRegistry.h"

// Hi, for recompilation.
bool Application::Init(int argc, char* argv[])
{
    if ( !SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) )
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Application.Init: SDL_Init failed: %s", SDL_GetError());
        return false;
    }

    if ( !TTF_Init() )
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Application.Init: TTF_Init failed: %s", SDL_GetError());
        return false;
    }

    ModManager& modManager = ModManager::GetInstance();
    modManager.ScanForMods();

    std::string targetedMod = ConstantConfiguration::defaultGameDirectoryName;
    for (int i = 1; i < argc; ++i)
    {
        if ((std::string(argv[i]) == "-game" || std::string(argv[i]) == "-mod") && i + 1 < argc)
        {
            targetedMod = argv[i + 1];
            break;
        }
    }

    if (modManager.HasMod(targetedMod)) 
    {
        modManager.SetActiveModConfiguration(targetedMod);
    }
    else
    {
        modManager.SetActiveModConfiguration(ConstantConfiguration::defaultGameDirectoryName);
    }

    if ( !Configuration::LoadFromFile(IOConfiguration::GetConfigFilePath()) )
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Application.Init: Configuration Init failed.");
        return false;
    }

    if ( !SettingsManager::GetInstance().LoadFromFile() )
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Application.Init: Settings manager initialization failed");
    }

    LOGGING_INIT();

    if ( !FlagManager::GetInstance().LoadFlagsFromFile() )
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Application.Init: FlagManager load from file failed.");
        return false;
    }

    if ( !AppRenderSystem::GetInstance().Init() )
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Application.Init: AppRenderSystem Init failed.");
        return false;
    }

    if ( !Assets::Init(AppRenderSystem::GetInstance().GetRenderer()) )
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Application.Init: Asset initialization failed: %s", SDL_GetError());
        return false;
    }

    if ( !TileRegistry::LoadTilesFromFile() )
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Application.Init: Tile type initialization failed: %s", SDL_GetError());
        return false;
    }

    if ( !AudioSystem::GetInstance().Init() )
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Application.Init: Audio system initialization failed: %s", SDL_GetError());
        return false;
    }

    if ( !SystemValueRegistry::GetInstance().LoadSystemValuesFromFile() )
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Application.Init: System value registry initialization failed");
    }

    if ( !FontManager::GetInstance().LoadFontsFromFile() )
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Application.Init: Font manager initialization failed");
        return false;
    }

    SystemActionRegistry::GetInstance().InitialiseSystemActions();
    GameActionRegistry::GetInstance().InitialiseGameActions();
    DataProvider::GetInstance().InitialiseProviders();
    SystemValueRegistry::GetInstance().InitialiseGetters();

    return true;
}

void Application::Run()
{
    AppStateManager::GetInstance().ChangeState(ENG_NEW(MainMenuState));
    AppStateManager::GetInstance().Run();
}

void Application::Cleanup()
{
    SystemValueRegistry::GetInstance().SaveSystemValuesToFile();
    LOGGING_CLEANUP();
    AudioSystem::GetInstance().Shutdown();
    Assets::Cleanup();
    FontManager::GetInstance().Cleanup();
    AppRenderSystem::GetInstance().Cleanup();
    AppStateManager::GetInstance().Cleanup();
    TileRegistry::Cleanup();
    TTF_Quit();
    SDL_Quit();
}