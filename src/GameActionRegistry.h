#pragma once

#ifndef EVENTMANAGER_H_
#define EVENTMANAGER_H_

#include <string>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>

/**
* @brief Simple manager for events. Translates JSON events into lambdas.
*/
class GameActionRegistry
{
private:
    std::unordered_map<std::string, std::function<void(const nlohmann::json&)>> handlers;  /// Map of [actionName; function].

    /**
    * @brief Constructor.
    */
    GameActionRegistry() = default;

    /**
    * @brief Default destructor.
    */
    ~GameActionRegistry() = default;

    // No copying or moving due to singleton pattern.
    GameActionRegistry(const GameActionRegistry&) = delete;
    GameActionRegistry& operator=(const GameActionRegistry&) = delete;
    GameActionRegistry(GameActionRegistry&&) = delete;
    GameActionRegistry& operator=(GameActionRegistry&&) = delete;


public:

    /**
    * @brief Returns a reference to the game action registry instance.
    * 
    * @return A reference to the game action registry instance.
    */
    static GameActionRegistry& GetInstance() noexcept
    {
        static GameActionRegistry instance;
        return instance;
    }

    /**
    * @brief Initialises the default game actions.
    */
    void InitialiseGameActions();

    /**
    * @brief Registers a handler for a game action with a certain name.
    * 
    * @param name Name of the action.
    * @param callback Function to call when the event is called.
    */
    void RegisterHandler(const std::string& name, std::function<void(const nlohmann::json&)> callback);

    /**
    * @brief Triggers a certain event.
    * 
    * @param name Name of the event.
    * @param params Parameters of the event.
    */
    void Execute(const std::string& name, const nlohmann::json& params = nlohmann::json()) const;
};

#endif // EVENTMANAGER_H_
