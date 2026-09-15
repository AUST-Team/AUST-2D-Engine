#pragma once

#ifndef TRIGGER_H_
#define TRIGGER_H_

#include <SDL3/SDL_rect.h>
#include <string>
#include <nlohmann/json.hpp>

#include "TriggerFlags.h"
#include "MapUpdateFlags.h"
#include "SpatialBuckets.h"
#include "TriggerState.h"
#include "FNV1aHash.h"
#include "SystemAction.h"

class ActivationExpression;
class Player;

// This is the third Trigger rewrite, please.
// Fourth ):
// Fifth!

/**
* @brief Data object for a trigger.
* 
* Trigger owns its activation expression.
*/
class Trigger
{
private:
    std::string triggerName;   /// Name of the trigger.
    SDL_Point position = { 0, 0 };  /// Position (tile indexes) of the interactable on the map.
    std::vector<SystemAction> events; /// Vector of system (and game) actions.
    ActivationExpression* activationExpression = nullptr;   /// Pointer to the activation expression. OWNS!
    uint64_t lastProcessedFrame = 0;    /// The last processed frame of the trigger.
                                        /// Compared with the currentFrameID in 'Map' in order to not fire a trigger multiple times.
    TriggerState triggerState;  /// The state of the trigger at any given point.
    HashID triggerId = 0;     /// ID of the trigger.
    TriggerFlags flags = TriggerFlags::None;    /// Trigger flags of the trigger.
    nlohmann::json templateJSON = nlohmann::json::object(); /// Template JSON of the trigger. Only used for dynamically created triggers.
                                                            /// Holds the definition of the trigger as described in the map JSON.

    // No copying due to polymorphic copying.
    Trigger(const Trigger&) = delete;
    Trigger& operator=(const Trigger&) = delete;

    /**
    * @brief Triggers the trigger's events.
    */
    void TriggerEvents();

public:

    /**
    * @brief Constructor.
    *
    * @param name Name of the trigger.
    * @param xPosition X coordinate of the tile where the trigger is located.
    * @param yPosition Y coordinate of the tile where the trigger is located.
    * @param eventsVec Vector of system actions to dispatch once triggered.
    * @param activationExpression The activation expression of the trigger. OWNS IT!
    * @param triggerFlags Trigger flags for the trigger.
    * @param origin Origin of the trigger.
    */
    explicit Trigger(
        const std::string& name,
        int xPosition = 0, 
        int yPosition = 0, 
        std::vector<SystemAction> eventsVec = {},
        ActivationExpression* activationExpression = nullptr, 
        TriggerFlags triggerFlags = TriggerFlags::AutoDelete,
        TriggerOrigin origin = TriggerOrigin::Static
    );

    /**
    * @brief Constructor.
    *
    * @param name Name of the trigger.
    * @param tilePosition Position of the tile where the trigger is located.
    * @param eventsVec Vector of system actions to dispatch once triggered.
    * @param activationExpression The activation expression of the trigger. OWNS IT!
    * @param triggerFlags Trigger flags for the trigger.
    * @param origin Origin of the trigger.
    */
    Trigger(
        const std::string& name = "",
        const SDL_Point& tilePosition = { 0, 0 },
        std::vector<SystemAction> eventsVec = {},
        ActivationExpression* activationExpression = nullptr, 
        TriggerFlags triggerFlags = TriggerFlags::AutoDelete,
        TriggerOrigin origin = TriggerOrigin::Static
    );

    /**
    * @brief Move constructor.
    */
    Trigger(Trigger&& other) noexcept;

    /**
    * @brief Destructor.
    */
    ~Trigger();

    /**
    * @brief Attempts to fire the trigger.
    * 
    * @param player Pointer to the player. NON-OWNING.
    * @param updateMask Update flags. A trigger will not fire if its update flags are not found.
    * 
    * @return 'true' if the trigger has fired, 'false' if otherwise.
    */
    bool TryFire(Player* player, MapUpdateFlags updateMask);

    /**
    * @brief Sets or unsets flags.
    *
    * @param triggerFlags Flags to set or unset.
    * @param enable 'true' if the flags should be set to '1', 'false' if the flag should be set to '0'.
    */
    void SetTriggerFlags(TriggerFlags triggerFlags, bool enable = true);

    /**
    * @brief Returns the trigger's flags.
    * 
    * @return The trigger's flags.
    */
    TriggerFlags GetTriggerFlags() const;

    /**
    * @brief Returns the trigger's activation expression's update flags.
    * 
    * @return The activation expression's update flags.
    */
    MapUpdateFlags GetUpdateFlags() const;

    /**
    * @brief Sets the triggered state of the trigger.
    * 
    * The trigger will automatically be set to triggered once it is fired!
    * 
    * @param triggered The new triggered state of the trigger; 'true' if it should be triggered, 'false' if it should not.
    */
    void SetTriggeredState(bool triggered);

    /**
    * @brief Returns the ID of the trigger.
    * 
    * @return The ID of the trigger.
    */
    HashID GetTriggerId() const;

    /**
    * @brief Returns the name of the trigger.
    * 
    * @return The name of the trigger.
    */
    std::string GetTriggerName();

    /**
    * @brief Checks if the trigger should be deleted.
    *
    * @return 'true' if the trigger should be deleted, 'false' if otherwise.
    */
    bool ShouldAutoDelete() const;

    /**
    * @brief Returns the events of the trigger.
    * 
    * @return Constant reference to the vector of events.
    */
    const std::vector<SystemAction>& GetEvents() const;

    /**
    * @brief Sets a new vector of events for the trigger.
    *
    * @param newEvents New vector of events. Will overwrite the previous one.
    */
    void SetEvents(const std::vector<SystemAction>& newEvents);

    /**
    * @brief Adds an event to the vector of events.
    *
    * @param event Event to be added
    */
    void AddEvent(const SystemAction& event);

    /**
    * @brief Returns the activation expression of the trigger.
    * 
    * @return Pointer to the activation expression of the trigger.
    */
    ActivationExpression* GetActivatorExpression() const;

    /**
    * @brief Sets a new activation expression for the trigger.
    * 
    * @param newExpression New activation expression for the trigger. OWNS IT!
    */
    void SetActivatorExpression(ActivationExpression* newExpression);

    /**
    * @brief Returns the tile position of the trigger.
    *
    * @return The tile position of the trigger.
    */
    SDL_Point GetPosition() const;

    /**
    * @brief Sets a new tile position for the trigger.
    *
    * @param x X coordinate of the new tile position.
    * @param y Y coordinate of the new tile position.
    */
    void SetPosition(int x, int y);

    /**
    * @brief Sets a new tile position for the trigger.
    *
    * @param newPosition New tile position for the trigger.
    */
    void SetPosition(const SDL_Point& newPosition);

    /**
    * @brief Sets the last processed frame of the trigger.
    * 
    * @param newFrame Frame the trigger was processed on.
    */
    void SetLastProcessedFrame(uint64_t newFrame);

    /**
    * @brief Returns the last frame that the trigger was processed.
    * 
    * @return The last frame the trigger was processed.
    */
    uint64_t GetLastProcessedFrame() const;

    /**
    * @brief Returns the spatial bucket of the trigger.
    *
    * @param Spatial bucket of the trigger.
    */
    SpatialBucket GetSpatialBucket() const;

    /**
    * @brief Returns the trigger's JSON template.
    * 
    * @return A reference to the trigger's JSON template.
    */
    const nlohmann::json& GetTemplateJSON() const;

    /**
    * @brief Sets the trigger's JSON template.
    * 
    * NOT NEEDED FOR TRIGGERS CREATED WHEN THE MAP FIRST LOADS!
    * ONLY FOR TRIGGERS CREATED DYNAMICALLY (setTriggers, events).
    * 
    * @param newTemplate New JSON template for the trigger.
    */
    void SetTemplateJSON(const nlohmann::json& newTemplate);

    /**
    * @brief Sets the origin of the trigger.
    * 
    * @param newOrigin New origin of the trigger.
    */
    void SetTriggerOrigin(TriggerOrigin newOrigin);

    /**
	* @brief Returns the last time the trigger was activated.
    * 
	* @return The last time the trigger was activated.
    */
	uint64_t GetLastActivatedTime() const;

    /**
    * @brief Returns the time the trigger was created.
     * 
	 * @return The time the trigger was created.
    */
    uint64_t GetCreatedTime() const;

    /**
	* @brief Returns the number of times the trigger has been activated.
    * 
	* @return The number of times the trigger has been activated.
    */
    int GetActivationCount() const;

    /**
    * @brief Converts the trigger RUNTIME STATE to JSON.
    *
    * @return JSON containing the trigger RUNTIME state.
    */
    nlohmann::json SerializeRuntimeState() const;

    /**
    * @brief Applies a saved runtime state to the trigger.
    * 
    * @param runtimeState JSON containing the runtime state.
    */
    void ApplyRuntimeState(const nlohmann::json& runtimeState);

    /**
    * @brief Checks if the trigger has fired.
    * 
    * @return 'true' if the triggered has fired, 'false' if otherwise.
    */
    bool IsTriggered() const;

    /**
    * @brief Checks if the trigger is repeatable.
    *
    * @return 'true' if the triggered is repeatable, 'false' if otherwise.
    */
    bool IsRepeatable() const;

    /**
    * @brief Checks if the trigger has been created dynamically or not.
    * 
    * @return 'true' if the trigger has been created dynamically, 'false' if otherwise.
    */
    bool IsRuntime() const;

    /**
    * @brief Move operator.
    */
    Trigger& operator=(Trigger&& other) noexcept;
};

#endif // TRIGGER_H_
