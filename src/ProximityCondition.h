#pragma once

#ifndef PROXIMITYCONDITION_H_
#define PROXIMITYCONDITION_H_

#include "Condition.h"

/**
* @brief Condition that is satisfied when the player is in 'range' tiles of the trigger.
* 
* Shareable.
*/
class ProximityCondition final : public Condition
{
private:
    uint64_t range;

public:

    /**
    * @brief Constructor.
    * 
    * @param range Range of the condition.
    */
    explicit ProximityCondition(uint64_t range);

    /**
    * @brief Default destructor.
    */
    ~ProximityCondition() override = default;

    /**
    * @brief Tests whether the 'in range' condition is satisfied.
    *
    * @param trigger Pointer to the trigger being evaluated. NON OWNING!
    * @param player Pointer to the player. NON OWNING!
    * @param updateMask Update flags. A condition will not be satisfied if its update flag is not found.
    *
    * @return 'true' if condition is satisfied, 'false' if otherwise.
    */
    bool IsSatisfied(Trigger* trigger, Player* player, MapUpdateFlags updateMask) const override;

    /**
    * @brief Returns which map update flags this condition depends on.
    *
    * @return The 'PlayerMoved' update flag.
    */
    MapUpdateFlags GetUpdateFlags() const override;

    /**
    * @brief Returns the spatial radius of the proximity condition.
    *
    * @return The proximity range of the condition.
    */
    uint64_t GetSpatialRadius() const override;

    /**
	* @brief Returns if the condition can be shared or not.
    * 
	* @return 'true' for proximity conditions.
    */
    bool IsShared() const override;
};

#endif // PROXIMITYCONDITION_H_