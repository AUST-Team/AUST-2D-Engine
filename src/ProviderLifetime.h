#pragma once

#ifndef PROVIDERLIFETIME_H_
#define PROVIDERLIFETIME_H_

#include <cstdint>

/**
* @brief Enumeration of possible lifetimes a data provider could have.
*/
enum class ProviderLifetime : uint8_t
{
    Persistent,     /// Persistent providers.
    OneTimeUse,     /// One time use, after being used, the key and provider are deleted.
    COUNT           /// Number of elements.
};

#endif // PROVIDERLIFETIME_H_