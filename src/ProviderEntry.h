#pragma once

#ifndef PROVIDERENTRY_H_
#define PROVIDERENTRY_H_

#include <functional>
#include <vector>
#include <string>
#include <variant>

#include "ProviderLifetime.h"

using DataVariant = std::variant<
    std::monostate,
    bool,
    int,
    float,
    std::string,
    std::vector<std::vector<std::string>>
>;

using ProviderFunc = std::function<DataVariant(void)>;

/**
* @brief An entry for a data provider.
*/
struct ProviderEntry
{
    ProviderFunc func; /// Provider function.
    ProviderLifetime lifetime = ProviderLifetime::Persistent;   /// Provider's lifetime.
};

#endif // PROVIDERENTRY_H_