#pragma once

#ifndef TRIGGERREGISTRY_H_
#define TRIGGERREGISTRY_H_

#include <unordered_map>
#include <array>
#include <vector>

#include "SpatialBuckets.h"
#include "MapUpdateFlags.h"
#include "FNV1aHash.h"

class Trigger;
class Player;

/**
* @brief Holds triggers, indexed by flags, and spatial buckets.
*/
class TriggerRegistry
{
private:
    std::unordered_map<HashID, Trigger*> triggersById;    /// Trigger by ID (hashed name). NOT OWNED!
    std::unordered_map<
        MapUpdateFlags,
        std::array<std::vector<Trigger*>, ToIndex(SpatialBucket::COUNT)>
    > triggersByFlag;   /// Map of triggers by update flag and categorized by the spatial bucket; 
                        /// [MapUpdateFlag; array<vector<Trigger *>>]. NOT OWNED. ONLY REFERENCED!
    uint64_t currentFrameID = 0;    /// The frame the update is currently on. Not a global timer, only used for frame processing. 

    /**
    * @brief Returns the Spatial bucket for an update mask.
    * 
    * @param updateMask Update flags to get the spatial bucket for.
    * 
    * @return The SpatialBucket corresponding to the update.
    */
    SpatialBucket GetMaxBucketForUpdate(MapUpdateFlags updateMask);

    // No copying due to Trigger being unable to be copied.
    TriggerRegistry(const TriggerRegistry&) = delete;
    TriggerRegistry& operator=(const TriggerRegistry&) = delete;

public:

    /**
    * @brief Default constructor.
    */
    TriggerRegistry() = default;

    /**
    * @brief Default move constructor.
    */
    TriggerRegistry(TriggerRegistry&& other) = default;

    /**
    * @brief Default destructor.
    * 
    * IT DOES NOT OWN THE POINTERS!
    */
    ~TriggerRegistry() = default;

    /**
    * @brief Processes the triggers.
    *
    * @param updateFlags Update flags to process.
    * @param player Pointer to the player.
    */
    void ProcessTriggers(MapUpdateFlags updateMask, Player* player);

    /**
    * @brief Registers a trigger in the registry.
    * 
    * @param trigger Pointer to the trigger. NON OWNING!
    * 
    * @return 'true' if the trigger was registered successfully, 'false' if otherwise.
    */
    bool RegisterTrigger(Trigger* trigger);

    /**
    * @brief Removes a trigger from the registry.
    * 
    * DOES NOT DELETE IT.
    * 
    * @param trigger Pointer to the trigger. NON OWNING.
    */
    void RemoveTrigger(Trigger* trigger);

    /**
    * @brief Returns a trigger by ID.
    *
    * @param triggerID The ID of the trigger.
    * 
    * @return Pointer to the trigger, or nullptr if it doesn't exist.
    */
    Trigger* GetTriggerById(HashID triggerID);

    /**
    * @brief Default move operator.
    */
    TriggerRegistry& operator=(TriggerRegistry&& other) = default;
};

#endif // TRIGGERREGISTRY_H_