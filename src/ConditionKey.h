#pragma once

#ifndef ACTIVATORKEY_H_
#define ACTIVATORKEY_H_

#include <nlohmann/json.hpp>
#include <variant>
#include <cstdint>

#include "Direction.h"
#include "MapUpdateFlags.h"
#include "ConditionType.h"
#include "FacingConditionType.h"
#include "TimedConditionType.h"
#include "TimedConditionParams.h"

/// Variant for the condition parameters. Use structures if there are more complex data types needed.
using ConditionParams = std::variant<
    std::monostate,         /// No parameters.
    FacingConditionType,    /// Facing type for facing condition.
    TimedConditionParams,   /// Timed condition parameters.
	uint64_t                /// Time for cooldown condition or max-activations for repeatable conditions or range for proximity conditions.
>;

/**
* @brief Structure for an condition key. Describes an condition.
* 
* It is used to refer to base condition (not composite ones!) with the same parameters / type / updateFlags.
*/
struct ConditionKey
{
    ConditionType type = ConditionType::COUNT;          /// Type of condition.
    MapUpdateFlags updateFlags = MapUpdateFlags::None;  /// Update flags of the corresponding condition.
    ConditionParams params = std::monostate{};          /// Parameters for the corresponding condition.

    /**
    * @brief Creates an condition key from JSON parameters.
    * 
    * @param conditionJSON JSON containing the condition parameters.
    * 
    * @return The ConditionKey created.
    */
    static ConditionKey ConditionKeyFromJSON(const nlohmann::json& conditionJSON);

    /**
    * @brief Returns the activator parameters from the JSON.
    * 
    * @param conditionJSON JSON containing the condition parameters.
    * @param type Type of activator.
    * 
    * @return ConditionParams of the corresponding type.
    */
    static ConditionParams ConditionParamsFromJSON(const nlohmann::json& conditionJSON, ConditionType type);

    /**
    * @brief Is-equal operator.
    */
    bool operator==(const ConditionKey& other) const = default;
};

// Tells the compiler how to hash an condition key.
namespace std 
{
    template <>
    struct hash<TimedConditionParams>
    {
        size_t operator()(const TimedConditionParams& p) const noexcept
        {
            size_t h = std::hash<uint8_t>()(static_cast<uint8_t>(p.type));

            h ^= std::hash<uint64_t>()(p.startTime) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<uint64_t>()(p.endTime) + 0x9e3779b9 + (h << 6) + (h >> 2);

            return h;
        }
    };

    template <>
    struct hash<ConditionKey> 
    {
        size_t operator()(const ConditionKey& k) const noexcept
        {
            size_t h = std::hash<int>()(static_cast<int>(k.type));

            h ^= std::hash<MapUpdateFlagsType>()(static_cast<MapUpdateFlagsType>(k.updateFlags)) << 1;

            // I love C++.
            std::visit([&](auto&& arg) {
                // HALF LIFE DECAY REFERENCE!
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!std::is_same_v<T, std::monostate>)
                {
                    h ^= std::hash<T>()(arg) << 2;
                }
            }, k.params);

            return h;
        }
    };
}

#endif // ACTIVATORKEY_H_
