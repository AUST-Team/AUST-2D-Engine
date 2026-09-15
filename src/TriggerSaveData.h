#pragma once

#ifndef TRIGGERSAVEDATA_H_
#define TRIGGERSAVEDATA_H_

#include <string>
#include <nlohmann/json.hpp>

#include "FNV1aHash.h"

/**
* @brief Save data of a trigger.
*
* 'Static' triggers only require runtimeState information, since they are created at map load.
*
* 'Dynamic' triggers require the template data (position, conditions, etc) too, since they are not created at map load.
*/
struct TriggerSaveData
{
    std::string name;   /// Name of the trigger.
    HashID id = 0;      /// Hashed trigger ID.
    bool isRuntimeFlag = false;     /// Flag if the trigger was created at dynamic or at map load.
    bool existsFlag = true;         /// Flag if the trigger exists or not.
    nlohmann::json templateData;    /// Template data for dynamic triggers.
    nlohmann::json runtimeState;    // Runtime data for the triggers (triggeredFlag, number of times interacted, etc). Shared by both.

    /**
    * @brief Converts the trigger save data to JSON format.
    *
    * @return a JSON containing the trigger save data.
    */
    nlohmann::json ToJSON() const;

    /**
    * @brief Converts a JSON format to trigger save data.
    *
    * @param triggerJSON JSON containing the trigger data in JSON format.
    *
    * @return A TriggerSaveData structure containing the trigger data.
    */
    static TriggerSaveData FromJSON(const nlohmann::json& triggerJSON);
};

#endif // TRIGGERSAVEDATA_H_