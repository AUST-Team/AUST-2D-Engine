#pragma once

#ifndef TRIGGERSTATE_H_
#define TRIGGERSTATE_H_

#include "TriggerOrigin.h"

/**
* @brief Structure for a trigger state.
*/
struct TriggerState
{
    uint64_t lastActivatedTime = 0;     /// The last moment the trigger was activated.
	uint64_t createdTime = 0;           /// The moment the trigger (and thus, activation expression and conditions) was created.
    int timesActivated = 0;         /// Number of times the trigger has been activated.
    bool triggeredFlag = false;     /// Flag if the trigger has been fired or not.
    TriggerOrigin origin = TriggerOrigin::Static;  /// The origin of the trigger.
};

#endif // TRIGGERSTATE_H_