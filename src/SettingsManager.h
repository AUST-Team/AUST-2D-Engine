#pragma once

#ifndef SETTINGSMANAGER_H_
#define SETTINGSMANAGER_H_

#include <cstdint>
#include <string>
#include <nlohmann/json.hpp>
#include <filesystem>

#include "Configuration.h"

/**
* @brief Settings for the engine or other misc
*/
class SettingsManager
{
private:

    /**
    * @brief Bitwise structure of setting flags.
    */
    struct SettingsFlags
    {
        uint8_t useLogging : 1;
        uint8_t logToFile : 1;
        uint8_t logToConsole : 1;
        uint8_t playerInvisible : 1;
        uint8_t supressWrapTextWarning : 1;

        SettingsFlags() :
            useLogging(1),
            logToFile(1),
            logToConsole(1),
            playerInvisible(1),
            supressWrapTextWarning(0) {}
    } settingsFlags;

    /**
    * @brief Loads settings from a JSON.
    * 
    * @param settingsJSON JSON containing the settings.
    * 
    * @return 'true' if zero or more flags have been loaded, 'false' if the JSON cannot be parsed.
    */
    bool LoadFromJSON(const nlohmann::json& settingsJSON);

public:

    /**
    * @brief Returns the instance of the settings manager.
    * 
    * @return Reference to the instance of settings manager.
    */
    static SettingsManager& GetInstance() 
    {
        static SettingsManager instance;
        return instance;
    }

    /**
    * @brief Loads the settings from a file.
    *
    * @param filePath The path to the file.
    *
    * @return 'true' if zero or more flags have been loaded, 'false' if file cannot be opened, or file cannot be parsed.
    */
    bool LoadFromFile(const std::filesystem::path& filePath = Configuration::Get().paths.settingsFilePath);

    /**
    * @brief Returns the 'useLogging' flag's value.
    * 
    * @return The state of the flag.
    */
    bool CheckUseLogging() const;

    /**
    * @brief Returns the 'logToFile' flag's value.
    *
    * @return The state of the flag.
    */
    bool CheckLogToFile() const;

    /**
    * @brief Returns the 'logToConsole' flag's value.
    *
    * @return The state of the flag.
    */
    bool CheckLogToConsole() const;

    /**
    * @brief Returns the 'supressWrapTextWarning' flag's value.
    *
    * @return The state of the flag.
    */
    bool CheckSupressWrapTextWarning() const;

    /**
    * @brief Returns the 'playerInvisible' flag's value.
    *
    * @return The state of the flag.
    */
    bool ShouldPlayerBeInvisible() const;

    /**
    * @brief Sets the value for the 'useLogging' flag.
    * 
    * @param newVal New value for the flag.
    */
    void SetUseLogging(bool newVal);

    /**
    * @brief Sets the value for the 'logToFile' flag.
    *
    * @param newVal New value for the flag.
    */
    void SetLogToFile(bool newVal);

    /**
    * @brief Sets the value for the 'logToConsole' flag.
    *
    * @param newVal New value for the flag.
    */
    void SetLogToConsole(bool newVal);

    /**
    * @brief Sets the value for the 'supressWrapTextWarning' flag.
    *
    * @param newVal New value for the flag.
    */
    void SetSupressWrapTextWarning(bool newVal);

    /**
    * @brief Sets the value for the 'playerInvisible' flag.
    *
    * @param newVal New value for the flag.
    */
    void SetPlayerInvisible(bool newVal);

    bool LoadFromFile(const std::string& filePath) = delete;
    bool LoadFromFile(const char * filePath) = delete;
};

#endif // SETTINGSMANAGER_H_