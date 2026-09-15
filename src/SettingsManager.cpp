#include <SDL3/SDL_log.h>

#include "SettingsManager.h"

#include "FileLoader.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

bool SettingsManager::LoadFromFile(const fs::path& filePath)
{
	const std::optional<json> jsonOpt = FileLoader::LoadJSON(filePath, JSONParseFlags::None);

	if (!jsonOpt)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SettingsManager.LoadFromFile: Failed to open file at %s.", filePath.string().c_str());
		return false;
	}

	return LoadFromJSON(*jsonOpt);
}

bool SettingsManager::CheckUseLogging() const { return settingsFlags.useLogging; }

bool SettingsManager::CheckLogToFile() const { return settingsFlags.logToFile; }

bool SettingsManager::CheckLogToConsole() const { return settingsFlags.logToConsole; }

bool SettingsManager::CheckSupressWrapTextWarning() const { return settingsFlags.supressWrapTextWarning; }

bool SettingsManager::ShouldPlayerBeInvisible() const { return settingsFlags.playerInvisible; }

void SettingsManager::SetUseLogging(bool newVal) { settingsFlags.useLogging = newVal; }

void SettingsManager::SetLogToFile(bool newVal) { settingsFlags.logToFile = newVal; }

void SettingsManager::SetLogToConsole(bool newVal) { settingsFlags.logToConsole = newVal; }

void SettingsManager::SetSupressWrapTextWarning(bool newVal) { settingsFlags.supressWrapTextWarning = newVal; }

void SettingsManager::SetPlayerInvisible(bool newVal) { settingsFlags.playerInvisible = newVal; }

bool SettingsManager::LoadFromJSON(const json& settingsJSON)
{
    /*
        What this function expects:

        Note that settingsJSON is the default JSON object.

        {
            "logging": {
                "useLogging": true,
                "logToFile": true,
                "logToConsole": true,
                "supressWrapTextWarning": true
            },
            "debug": {
                "shouldPlayerBeInvisible": false
            }
        }
    */

    if (!settingsJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SettingsManager.LoadFromJSON: Settings JSON is not object.");
        return false;
    }

    if (!settingsJSON.contains("logging") || !settingsJSON["logging"].is_object())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SettingsManager.LoadFromJSON: Settings JSON doesn't contain logging or is not object. Defaulting to all true.");
    }

    const json loggingJSON = settingsJSON.value("logging", json::object());

    settingsFlags.useLogging = loggingJSON.value("useLogging", true);
    settingsFlags.logToFile = loggingJSON.value("logToFile", true);
    settingsFlags.logToConsole = loggingJSON.value("logToConsole", true);
    settingsFlags.supressWrapTextWarning = loggingJSON.value("supressWrapTextWarning", false);

    if (settingsJSON.contains("debug")) 
    {
        const json& debug = settingsJSON["debug"];
        settingsFlags.playerInvisible = debug.value("shouldPlayerBeInvisible", false);
    }

    return true;
}