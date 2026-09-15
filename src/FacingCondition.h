#pragma once

#ifndef FACINGCONDITION_H_
#define FACINGCONDITION_H_

#include "Condition.h"
#include "FacingConditionType.h"

/**
* @brief Condition for whether the player is facing the trigger or not.
* 
* Shareable.
* 
* Use in combination with other conditions.
*/
class FacingCondition final : public Condition
{
private:
    FacingConditionType facingType; /// The facing type of the condition.

public:

    /**
    * @brief Constructor.
    * 
    * @param type The facing type of the condition.
    */
    explicit FacingCondition(FacingConditionType type);

    /**
    * @brief Default destructor.
    */
    ~FacingCondition() override = default;

    /**
    * @brief Tests whether the 'facing' condition is satisfied.
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
    * @return The 'PlayerDirectionChange' update flag.
    */
    MapUpdateFlags GetUpdateFlags() const override;

    /**
    * @brief Returns the spatial radius of the facing condition.
    *
    * @return The default spatial radius of the facing condition.
    */
    uint64_t GetSpatialRadius() const override;

    /**
	* @brief Returns if the condition can be shared or not.
    * 
	* @return 'true' for facing conditions.
    */
	bool IsShared() const override;
};

#endif // FACINGCONDITION_H_