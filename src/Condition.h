#pragma once

#ifndef CONDITION_H_
#define CONDITION_H_

#include "MapUpdateFlags.h"

class Trigger;
class Player;

/**
* @brief Interface for a trigger condition.
*
* A condition answers: "Is this condition satisfied right now?"
*/
class Condition
{
public:

    /**
    * @brief Default destructor.
    */
    virtual ~Condition() = default;

    /**
    * @brief Tests whether the condition is satisfied.
    *
    * @param trigger Pointer to the trigger being evaluated. NON OWNING!
    * @param player Pointer to the player. NON OWNING!
    * @param updateMask Update flags. A condition will not be satisfied if its update flag is not found.
    *
    * @return 'true' if condition is satisfied, 'false' if otherwise.
    */
    virtual bool IsSatisfied(Trigger* trigger, Player* player, MapUpdateFlags updateMask) const = 0;

    /**
    * @brief Returns which map update flags this condition depends on.
    *
    * Used for efficient dispatch.
    * 
    * @return The MapUpdateFlags the condition depends on.
    */
    virtual MapUpdateFlags GetUpdateFlags() const = 0;

    /**
    * @brief Returns the spatial radius of the condition.
    * 
    * @return The spatial radius of the condition. It is used to determine if a condition should even be checked or not.
    * 0 means the player has to be standing on it. 1 is 1 tile away, and so on.
    * Default 0.
    */
    virtual uint64_t GetSpatialRadius() const = 0;

    /**
    * @brief Checks if the condition can be shared or not.
    * 
    * Shareable conditions are usually the base conditions (interact, proximity, etc), and non shareable ones are the composite ones (AND, OR, etc).
    * 
    * @return 'true' if can be shared, 'false' if otherwise.
    * Default 'true'.
    */
    virtual bool IsShared() const = 0;
};

#endif // CONDITION_H_