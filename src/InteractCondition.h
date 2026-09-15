#pragma once

#ifndef INTERACTCONDITION_H_
#define INTERACTCONDITION_H_

#include "Condition.h"

/**
* @brief Condition that is satisfied if the player is on an adjacenent 4-neighbor tile from the trigger and facing it.
* 
* Shareable.
* 
* Note that 'facing it' is not the same as FacingCondition. This is only so that interacting requires to face the object,
* whereas the FacingCondition only checks if the player is facing the trigger at any point.
*/
class InteractCondition final : public Condition
{
public:

    /**
    * @brief Default destructor.
    */
    ~InteractCondition() override = default;

    /**
    * @brief Tests whether the 'interact' condition is satisfied.
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
    * @return The 'Interact' update flag.
    */
    MapUpdateFlags GetUpdateFlags() const override;

    /**
    * @brief Returns the spatial radius of the interact condition.
    * 
    * @return The default spatial radius of the interact condition (in most cases, 1).
    */
    uint64_t GetSpatialRadius() const override;

    /**
	* @brief Returns if the condition can be shared or not.
    * 
	* @return 'true' for interact conditions.
    */
	bool IsShared() const override;
};

#endif // INTERACTCONDITION_H_