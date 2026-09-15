#pragma once

#ifndef MAXACTIVATIONSCONDITION_H_
#define MAXACTIVATIONSCONDITION_H_

#include "Condition.h"

/**
* @brief Condition for whether the trigger should be active based on the number of times it has been activated.
*/
class MaxActivationsCondition : public Condition
{
private:
	uint64_t maxActivations = 0;	/// The maximum number of activations for the trigger to be active.

public:

	/**
	* @brief Constructor for a max activations condition.
	* 
	* @param maxActivations The maximum number of activations for the trigger to be active.
	*/
	explicit MaxActivationsCondition(uint64_t maxActivations);

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
	* @return MapUpdateFlags::None, since this condition is only based on the trigger's activation count.
	*/
	MapUpdateFlags GetUpdateFlags() const override;

	/**
	* @brief Returns the spatial radius of the condition.
	* 
	* @return 0, since this condition is only based on the trigger's activation count and does not depend on the player's position.
	*/
	uint64_t GetSpatialRadius() const override;

	/**
	* @brief Checks if the condition can be shared or not.
	* 
	* @return 'false', since this condition depends on the trigger's activation count, which is different for each trigger.
	*/
	bool IsShared() const override;
};

#endif // MAXActivationsCondition_H_