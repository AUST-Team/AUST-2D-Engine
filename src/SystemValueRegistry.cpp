#include <SDL3/SDL_log.h>
#include <fstream>
#include <iostream>

#include "SystemValueRegistry.h"

#include "ConstantConfiguration.h"
#include "AppRenderSystem.h"
#include "Audio.h"
#include "FileLoader.h"
#include "JSONParser.h"

using json = nlohmann::json;

bool SystemValueRegistry::LoadSystemValuesFromJSON(const nlohmann::json& valuesJSON)
{
    /*
        What this function expects:

        Note that valuesJSON is the default JSON object.

        {
            "isFullscreen": true / false,
            "masterVolume": <float between 0.0 and 1.0>,
            "appDimensions": { "x": 800, "y": 600 }
        }
    
    */

    if (!valuesJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SystemValueRegistry.LoadSystemValuesFromJSON: Values JSON is not object.");
        return false;
    }

    // I, for the life of me, cannot make the window start fullscreen-ed without it looking horrible. It works just fine once everything is loaded
    // And the user taps the checkbox for the fullscreen, but if I try to pass it in the SDL_CreateWindow(...) it shows the magenta (which is the
    // RendererClear(...) color to show that something has gone terribly wrong) while also keeping the checkbox synced (so if it starts as fullscreen
    // the checkbox would be ticked already). If anyone knows how to do it properly, tell me.

    /*
    if (valuesJSON.contains("isFullscreen") && valuesJSON["isFullscreen"].is_boolean())
    {
        bool isFullscreen = valuesJSON.value("isFullscreen", false);
        AppRenderSystem::GetInstance().ScheduleFullScreen(isFullscreen);
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemValueRegistry.LoadSystemValuesFromJSON: Values JSON does not contain \"masterVolume\" or is not float. Defaulting to '1.0f'");
    }
    */

    if (valuesJSON.contains("masterVolume") && valuesJSON["masterVolume"].is_number_float())
    {
        float masterVolume = std::clamp(valuesJSON.value("masterVolume", 1.0f), 0.0f, 1.0f);
        Audio::SetMasterVolume(masterVolume);
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemValueRegistry.LoadSystemValuesFromJSON: Values JSON does not contain \"masterVolume\" or is not float. Defaulting to '1.0f'");
    }

    // Same here.
    /*
    const GameConfiguration& config = Configuration::Get();

    Size appSize = { 
        config.window.width,
        config.window.height
    };

    if (valuesJSON.contains("appDimensions") && (valuesJSON["appDimensions"].is_object() || valuesJSON["appDimensions"].is_array() || valuesJSON["appDimensions"].is_number()))
    {
        appSize = JSONParser::ParseJSONSize<Size>(valuesJSON["appDimensions"], appSize);
        AppRenderSystem::GetInstance().ScheduleWindowSizeChange(appSize);
    }
    else
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SystemValueRegistry.LoadSystemValuesFromJSON: Values JSON does not contain \"appDimensions\" or is not float. Defaulting to configuration settings.");
    }
    */

    return true;
}

bool SystemValueRegistry::LoadSystemValuesFromFile(const std::filesystem::path& filePath)
{
    std::optional<json> jsonOpt = FileLoader::LoadJSON(filePath);

    if (!jsonOpt)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SystemValueRegistry.LoadSystemValuesFromFile: Failed to load system value file at [%s]", filePath.string().c_str());
        return false;
    }

    return LoadSystemValuesFromJSON(*jsonOpt);
}

bool SystemValueRegistry::SaveSystemValuesToFile(const std::filesystem::path& filePath)
{
    std::ofstream file(filePath, std::ios::trunc);
    if (!file.is_open())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SystemValueRegistry.LoadSystemValuesFromFile: Failed to open file for writing: %s", filePath.string().c_str());
        return false;
    }

    json j;

    const AppRenderSystem& app = AppRenderSystem::GetInstance();

    j["isFullscreen"] = app.IsFullscreen();
    j["masterVolume"] = Audio::GetMasterVolume();
    j["appDimensions"] = { {"w", app.GetWidth()}, {"h", app.GetHeight()} };

    try
    {
        file << j.dump(4);
    }
    catch (const std::exception& e)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SystemValueRegistry.LoadSystemValuesFromFile: Exception at writing to file [%s]: %s", filePath.string().c_str(), e.what());
        return false;
    }

    return true;
}

void SystemValueRegistry::InitialiseGetters()
{
    RegisterGetter(ConstantConfiguration::isFullscreenValueKey, []() {
        return AppRenderSystem::GetInstance().IsFullscreen();
    });

    RegisterGetter(ConstantConfiguration::masterVolumeValueKey, []() {
        return Audio::GetMasterVolume();
    });

    RegisterGetter(ConstantConfiguration::appWidthValueKey, []() {
        return AppRenderSystem::GetInstance().GetWidth();
    });

    RegisterGetter(ConstantConfiguration::appHeightValueKey, []() {
        return AppRenderSystem::GetInstance().GetHeight();
    });
}

void SystemValueRegistry::RegisterGetter(const std::string& key, const ValueGetter& getter) { getters[key] = getter; }
