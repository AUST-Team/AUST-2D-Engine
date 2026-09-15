#pragma once

#ifndef TIMEDCONDITION_H_
#define TIMEDCONDITION_H_

#include "Condition.h"

#include "TimedConditionParams.h"

/**
* @brief Condition for whether the trigger should be active based on time.
*/
class TimedCondition : public Condition
{
private:
	TimedConditionParams params; /// Parameters for the timed condition.
	mutable uint64_t absoluteTargetTime = 0;	/// Cached absolute target time (MS) for the condition to be turned (in)active, or the start of the interval of time for the condition to be active..
	mutable uint64_t lastCheckedTime = 0;		/// The last time (MS) this condition was checked.
	mutable bool isInitializedFlag = false;		/// Flag if the timed condition has been initialized or not. Used to initialize the absolute target time on the first check of 'IsSatisfied'.

public:

	/**
	* @brief Constructor for a timed condition.
	* 
	* @param type Type of the timed condition, either active before or after a certain time (startTime), or in a given interval (startTime, endTime), or a specific time (startTime, endTime = startTime).
	* @param startTime Start time (MS) for the condition to be turned (in)active, or the start of the interval of time for the condition to be active.
	* @param endTime Only used for interval timed conditions, represents the end of the interval of time (MS) for the condition to be active.
	*/
	TimedCondition(TimedConditionType type, uint64_t startTime, uint64_t endTime);

	/**
	* @brief Constructor for a timed condition.
	* 
	* @param params The parameters for the timed condition.
	*/
	TimedCondition(const TimedConditionParams& params);

	/**
	* @brief Default destructor.
	*/
	~TimedCondition() override = default;

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
	* @brief Returns if the condition can be shared or not.
	* 
	* @return 'false' for timed conditions.
	*/
	bool IsShared() const override;
};

#endif // TIMEDCONDITION_H_