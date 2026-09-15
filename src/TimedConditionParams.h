#pragma once

#ifndef TIMEDCONDITIONPARAMS_H_
#define TIMEDCONDITIONPARAMS_H_

#include <cstdint>

#include "TimedConditionType.h"

/**
* @brief Structure for the parameters of a timed condition.
*/
struct TimedConditionParams
{
	TimedConditionType type = TimedConditionType::Before;   /// Type of the timed condition, either active before or after a certain time (startTime), 
	/// or in a given interval (startTime, endTime), or a specific time (startTime, endTime = startTime).
	uint64_t startTime = 0;    /// Start time (MS) for the condition to be turned (in)active, or the start of the interval of time for the condition to be active.
	uint64_t endTime = 0;      /// Only used for interval timed conditions, represents the end of the interval of time (MS) for the condition to be active.

	/**
	* @brief Default is-equal operator.
	*/
	bool operator==(const TimedConditionParams& other) const = default;
};

#endif // TIMEDCONDITIONPARAMS_H_