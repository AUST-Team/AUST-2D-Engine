#pragma once

#ifndef SYSTEMACTIONREGISTRY_H_
#define SYSTEMACTIONREGISTRY_H_

#include "SystemAction.h"

/// Alias for an action handler function that takes a JSON payload.
using ActionHandler = std::function<void(const nlohmann::json&)>;

/**
* @brief Singleton for registering and executing system action handlers.
*/
class SystemActionRegistry
{
private:
	std::unordered_map<std::string, ActionHandler> handlers;    /// Map of [actionName, ActionHandler].

    /**
    * @brief Default constructor.
	*/
    SystemActionRegistry() = default;

    /**
	* @brief Default destructor.
    */
	~SystemActionRegistry() = default;

	// No copying or moving due to singleton pattern.
    SystemActionRegistry(const SystemActionRegistry&) = delete;
    SystemActionRegistry& operator=(const SystemActionRegistry&) = delete;
    SystemActionRegistry(SystemActionRegistry&&) = delete;
	SystemActionRegistry& operator=(SystemActionRegistry&&) = delete;

public:

    /**
	* @brief Returns or creates an instance of the SystemActionRegistry.
    * 
	* @return Reference to the SystemActionRegistry instance.
    */
    static SystemActionRegistry& GetInstance() noexcept
    {
        static SystemActionRegistry instance;
        return instance;
    }

    /**
	* @brief Registers a handler for a specific action name.
    * 
	* @param actionName The name of the action to register the handler for.
	* @param handler The handler function to be called when the action is executed.
    */
    void RegisterHandler(const std::string& actionName, const ActionHandler& handler);

    /**
	* @brief Executes the handler for a specific action name with an optional JSON payload.
    * 
	* @param actionName The name of the action to execute.
	* @param payload Optional JSON payload to pass to the handler.
    */
    void Execute(const std::string& actionName, const nlohmann::json& payload = {});

    /**
	* @brief Executes the handler for a specific SystemAction.
    * 
	* @param action The SystemAction to execute.
    */
    void Execute(const SystemAction& action);

    /**
    * @brief Initialises the default system actions.
    */
    void InitialiseSystemActions();
};

#endif // SYSTEMACTIONREGISTRY_H_