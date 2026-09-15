#pragma once

#ifndef COOLDOWNCONDITION_H_
#define COOLDOWNCONDITION_H_

#include "Condition.h"

/**
* @brief Condition for whether the trigger should be active based on a cooldown time between activations.
*/
class CooldownCondition : public Condition
{
private:
	uint64_t cooldownTime; /// The cooldown time (MS) for the condition.

public:

	/**
	* @brief Constructor for a cooldown condition.
	* 
	* @param cooldownTime The cooldown time (MS) for the condition.
	*/
	explicit CooldownCondition(uint64_t cooldownTime);

	/**
	* @brief Default destructor.
	*/
	~CooldownCondition() override = default;

	/**
	* @brief Tests whether the condition is satisfied.
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
	* Used for efficient dispatch.
	* 
	* @return MapUpdateFlags::None, since this condition is only based on time.
	*/
	MapUpdateFlags GetUpdateFlags() const override;

	/**
	* @brief Returns the spatial radius of the condition.
	* 
	* @return 0, since this condition is only based on time and does not depend on the player's position.
	*/
	uint64_t GetSpatialRadius() const override;

	/**
	* @brief Returns whether the condition can be shared or not.
	* 
	* @return 'false' for the cooldown condition.
	*/
	bool IsShared() const override;
};

#endif // COOLDOWNCONDITION_H_