#pragma once

#ifndef TRIGGERORIGIN_H_
#define TRIGGERORIGIN_H_

#include <cstdint>

/**
* @brief Enumeration of trigger origins.
*/
enum class TriggerOrigin : uint8_t
{
    Static,     /// Loaded from map JSON.
    Runtime,    /// Created via setTrigger / events.
    COUNT       /// Number of elements
};

#endif // TRIGGERORIGIN_H_