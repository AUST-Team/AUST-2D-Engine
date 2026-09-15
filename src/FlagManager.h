#pragma once

#ifndef FLAGMANAGER_H_
#define FLAGMANAGER_H_

#include <vector>
#include <unordered_map>
#include <string>
#include <filesystem>

#include "Configuration.h"
#include "FNV1aHash.h"

/**
* @brief Class that holds the game flags as bits. Flags can be accessed by name.
* 
* Singleton pattern.
*/
class FlagManager 
{
private:
    std::unordered_map<HashID, std::string> flagIdToName;   /// A map of [flagId; name]. Used mainly for debug.
    std::unordered_map<HashID, size_t> flagIdToIndex;     /// A map of [flagId; position in vector].
    std::vector<uint64_t> flagBits; /// Vector of bits representing the flags.

    /**
    * @brief Constructor.
    */
    FlagManager() = default;

    /**
    * @brief Default destructor.
    */
    ~FlagManager() = default;

    // No copying or moving due to singleton pattern.
    FlagManager(const FlagManager&) = delete;
    FlagManager& operator=(const FlagManager&) = delete;
    FlagManager(FlagManager&&) = delete;
    FlagManager& operator=(FlagManager&&) = delete;

    /**
    * @brief Loads the flag from a JSON file.
    * 
    * @param flagsJSON JSON containing the definitions of the flags.
    * 
    * @return 'true' if zero or more flags have been loaded succesfully, 'false' otherwise.
    */
    bool LoadFlagsFromJSON(const nlohmann::json& flagsJSON);

public:

    /**
    * @brief Returns the instance of the game flag manager.
    * 
    * @return Reference to the instance of the game flag manager.
    */
    static FlagManager& GetInstance() noexcept
    {
        static FlagManager instance;
        return instance;
    }

    /**
    * @brief Loads the flags from a JSON file.
    *
    * @param filePath Path to the file containing the flag definitions.
    * 
    * @return 'true' if zero or more flags have been loaded succesfully, 'false' otherwise.
    */
    bool LoadFlagsFromFile(const std::filesystem::path& filePath = Configuration::Get().paths.flagFilePath);

    /**
    * @brief Defines a flag.
    * 
    * @param name Name of the flag.
    * @oaram initialValue Initial value of the flag.
    */
    void DefineFlag(const std::string& name, bool initialValue = false);

    /**
    * @brief Exports the flags as a hex string.
    * 
    * @return The game flags as a hex string.
    */
    std::string ExportAsHex() const;

    /**
    * @brief Imports (sets) the flag from a hex string.
    * 
    * @param hexString String containing the hex of the flags.
    */
    void ImportFromHex(const std::string& hexString);

    /**
    * @brief Sets the value of the flag.
    * 
    * @param name Name of the flag.
    * @param value New value for the flag.
    */
    void SetFlag(const std::string& name, bool value);

    /**
    * @brief Returns the value of a flag, or 'false' if the flag doesn't exist.
    * 
    * @param name Name of the flag.
    * 
    * @return The value of the flag, or 'false' if the flag doesn't exist.
    */
    bool GetFlag(const std::string& name) const;

    bool LoadFlagsFromFile(const std::string& filePath) = delete;
    bool LoadFlagsFromFile(const char * filePath) = delete;
};

#endif // FLAGMANAGER_H_
