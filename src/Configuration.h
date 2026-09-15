#pragma once

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <string>
#include <nlohmann/json.hpp>
#include <SDL3/SDL_pixels.h>
#include <filesystem>

#include "GameConfiguration.h"
#include "IOConfiguration.h"

/**
* @brief Static class for constants and configuration.
*/
class Configuration
{
private:
    static GameConfiguration config;    /// Configuration of the game.

    /**
    * @brief Loads the configuration from a JSON.
    * 
    * @param configurationJSON The JSON file containing the configuration.
    */
    static void LoadConfigurationFromJSON(const nlohmann::json& configurationJSON);

    // No instancing due to static class.
    Configuration() = delete;
    ~Configuration() = delete;

    // No copying or moving due to static class.
    Configuration(const Configuration&) = delete;
    Configuration& operator=(const Configuration&) = delete;
    Configuration(Configuration&&) = delete;
    Configuration& operator=(Configuration&&) = delete;

public:

    /**
    * @brief Returns the configuration of the game.
    *
    * @return Reference to the configurations of the game.
    */
    static const GameConfiguration& Get();

    /**
    * @brief Loads the configuration from a file.
    * 
    * @param path Path to the file.
    */
    static bool LoadFromFile(const std::filesystem::path& path);

    static bool LoadFromFile(const std::string& path) = delete;
    static bool LoadFromFile(const char * path) = delete;
};

#endif // CONFIGURATION_H_