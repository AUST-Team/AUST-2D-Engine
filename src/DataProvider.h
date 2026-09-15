#pragma once

#ifndef DATAPROVIDER_H_
#define DATAPROVIDER_H_

#include <optional>

#include "ProviderEntry.h"

/**
* @brief Abstracts where data provides from in tables.
*/
class DataProvider
{
private:
    std::unordered_map<std::string, ProviderEntry> providers;    /// Map of data providers by name.

    /**
    * @brief Default constructor.
    */
    DataProvider() = default;

    /**
    * @brief Default destructor.
    */
    ~DataProvider() = default;

    //No coyping or moving allowed due to singleton pattern.
    DataProvider(const DataProvider&) = delete;
    DataProvider& operator=(const DataProvider&) = delete;
    DataProvider(DataProvider&&) = delete;
    DataProvider& operator=(DataProvider&&) = delete;

public:

    /**
    * @brief Returns the instance of the data provider.
    * 
    * @return The instance of the data provider.
    */
    static DataProvider& GetInstance() 
    {
        static DataProvider instance;
        return instance;
    }

    /**
    * @brief Initialises the base providers.
    */
    void InitialiseProviders();

    /**
    * @brief Registers a data provider with a specified key.
    * 
    * @param key Key (~name) of the data provider.
    * @param provider Data provider entry (structure of both the function and lifetime)
    */
    void RegisterProvider(const std::string& key, const ProviderEntry& provider);

    /**
    * @brief Registers a data provider with a specified key.
    *
    * @param key Key (~name) of the data provider.
    * @param func The provider function.
    * @param lifetime The lifetime of the provider.
    */
    void RegisterProvider(const std::string& key, const ProviderFunc& func, ProviderLifetime lifetime = ProviderLifetime::Persistent);

    /**
    * @brief Fetches the data from a provider. The caller is responsible for the knowing the type.
    * 
    * @param key Key of the provider.
    * 
    * @return The data from the provider, or std::nullopt if the data doesn't exist.
    */
    template<typename T>
    std::optional<T> FetchData(const std::string& key)
    {
        auto it = providers.find(key);
        if (it != providers.end())
        {
            DataVariant resultVar = it->second.func();

            if (it->second.lifetime == ProviderLifetime::OneTimeUse)
            {
                providers.erase(it);
            }

            if (std::holds_alternative<T>(resultVar))
            {
                return std::get<T>(resultVar);
            }
            else
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DataProvider::FetchData: Type mismatch for key '%s'", key.c_str());
            }
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DataProvider::FetchData: Key '%s' not found", key.c_str());
        }

        return std::nullopt;
    }
};

#endif // DATAPROVIDER_H_