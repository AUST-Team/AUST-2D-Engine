#pragma once

#ifndef SYSTEMVALUEREGISTRY_H_
#define SYSTEMVALUEREGISTRY_H_

#include <functional>
#include <variant>
#include <string>
#include <unordered_map>

#include "Configuration.h"

using ValueGetterVariant = std::variant<int, bool, float, std::string>;
using ValueGetter = std::function<ValueGetterVariant()>;

/**
* @brief Registry for system values. Caller is responsible for knowing what a certain key will return.
*/
class SystemValueRegistry
{
private:
    std::unordered_map<std::string, ValueGetter> getters;   /// Map of [key, ValueGetter].

    /**
    * @brief Default constructor.
    */
    SystemValueRegistry() = default;

    /**
    * @brief Default destructor.
    */
    ~SystemValueRegistry() = default;

    //No coyping or moving allowed due to singleton pattern.
    SystemValueRegistry(const SystemValueRegistry&) = delete;
    SystemValueRegistry& operator=(const SystemValueRegistry&) = delete;
    SystemValueRegistry(SystemValueRegistry&&) = delete;
    SystemValueRegistry& operator=(SystemValueRegistry&&) = delete;

    /**
    * @brief Loads the system values from a JSON, and applies them. If file doesn't exist, then default values will be used.
    * 
    * @param valuesJSON JSON containing the values.
    * 
    * @return 'true' if loading was successful, 'false' otherwise.
    */
    bool LoadSystemValuesFromJSON(const nlohmann::json& valuesJSON);

public:

    /**
    * @brief Returns the instance of the system value registry.
    * 
    * @return Reference to the system value registry.
    */
    static SystemValueRegistry& GetInstance() 
    {
        static SystemValueRegistry instance;
        return instance;
    }

    /**
    * @brief Loads the system values from a file, and applies them. If file doesn't exist, then default values will be used.
    * 
    * @param filePath Path to the file.
    * 
    * @return 'true' if loading was successful, 'false' otherwise.
    */
    bool LoadSystemValuesFromFile(const std::filesystem::path& filePath = Configuration::Get().paths.systemValuesFilePath);

    /**
    * @brief Saves the system values to a file. If file doesn't exist, it will create it.
    * 
    * @param filePath Path to the file to save the system values.
    * 
    * @return 'true' if saving was succesful, 'false' otherwise.
    */
    bool SaveSystemValuesToFile(const std::filesystem::path& filePath = Configuration::Get().paths.systemValuesFilePath);

    /**
    * @brief Initialises the base system values.
    */
    void InitialiseGetters();

    /**
    * @brief Registers a getter with a given key.
    * 
    * @param key Key (~name) of the getter.
    * @param getter Function that returns a value (variant).
    */
    void RegisterGetter(const std::string& key, const ValueGetter& getter);

    /**
    * @brief Returns a value from a getter.
    * 
    * @param key Key of the getter.
    * @param defaultValue Default value if the getter doesn't exist or if the type returned by the getter and template type don't match.
    * 
    * @return The value of the getter, or 'defaultValue'.
    */
    template<typename T>
    T GetValue(const std::string& key, T defaultValue) const 
    {
        auto it = getters.find(key);
        if (it != getters.end()) 
        {
            ValueGetterVariant var = it->second();
            if (std::holds_alternative<T>(var)) 
            {
                return std::get<T>(var);
            }
        }
        return defaultValue;
    }

    bool LoadSystemValuesFromFile(const std::string& filePath) = delete;
    bool LoadSystemValuesFromFile(const char* filePath) = delete;
};

#endif // SYSTEMVALUEREGISTRY_H_