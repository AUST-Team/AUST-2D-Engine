#pragma once

#ifndef STEPPEDONCONDITION_H_
#define STEPPEDONCONDITION_H_

#include "Condition.h"

/**
* @brief Condition for if the trigger is stepped on by the player.
* 
* Shareable.
*/
class SteppedOnCondition final : public Condition
{
public:

    /**
    * @brief Default destructor.
    */
    ~SteppedOnCondition() override = default;

    /**
    * @brief Tests whether the 'stepped on' condition is satisfied.
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
    * @brief Returns the spatial radius of the condition.
    *
	* @return 0, since this condition activates only when the player is standing on the trigger.
    */
    uint64_t GetSpatialRadius() const override;

    /**
    * @brief Returns if the condition can be shared or not.
    *
    * @return 'true' for steppedOn conditions.
    */
    bool IsShared() const override;
};

#endif // STEPPEDONCONDITION_H_