#pragma once

#ifndef ENUMMISCS_H_
#define ENUMMISCS_H_

#include <type_traits>
#include <cstdint>
#include <cassert>

#include "ConstantConfiguration.h"

// Opt-in trat for enums that are countable.
// To opt-in:
//  template <typename T>
//  inline constexpr bool isCountableEnum<EnumName> = true;
template <typename T>
inline constexpr bool isCountableEnum = false;

// Requirement for countable enums, have a T::COUNT.
template <typename T>
concept CountableEnum = isCountableEnum<T> && requires { T::COUNT; };

// Trait to opt-in for enums with sentinel values. By default, no enum is considered a sentinel enum.
// To opt-in:
//  template <>
//  inline constexpr bool isSentinelEnum<EnumName> = true;
// The enum MUST have T::ENUM_SENTINEL_VALUE as the first value, starting at ConstantaConfiguration::enumSentinelValue.
// MUST be declared right after the enum class Name { ... } declaration, otherwise it will not work and the error message is 'isSentinelEnum is not a typename'.
template <typename T>
inline constexpr bool isSentinelEnum = false;

// I swear this makes sense.
// This is a concept (compile time requirement) that checks if a type is opted-in AND has a sentinel value (ENUM_SENTINEL_VALUE) and it has the value of ConstantaConfiguration::enumSentinelValue.
// If ANY of you reading this used Java or Kotlin generic programming, it's basically upper and lower bounds, in a way.
// Atleast now if the enum doesn't have T::ENUM_SENTINEL_VALUE, you'll get a better error message.
template <typename T>
concept SentinelEnum = isSentinelEnum<T> && requires 
{
    // Requires that T::ENUM_SENTINEL_VALUE is a valid expression in the enum.
    T::ENUM_SENTINEL_VALUE;
} && (static_cast<SentinelEnumType>(T::ENUM_SENTINEL_VALUE) == ConstantConfiguration::enumSentinelValue);

// Sentinel enum with COUNT. 
// This is a concept (compile time requirement) that checks if a type has sentinel AND is countable.
template <typename T>
concept CountableSentinelEnum = CountableEnum<T> && SentinelEnum<T>;

/**
* @brief Checks if a given enum type is undefined (equal to the sentinel value). Enum type must be opted-in for this operator to work.
* 
* Equal to the 'IsSentinel' function.
* 
* @param type The enum type to check.
* @param [template] T The enum type.
* @param [requires] SentinelEnum<T> The enum type must be opted-in AND have T::ENUM_SENTINEL_VALUE for this operator to work.
* 
* @return 'true' if the enum type is undefined (the sentinel value), 'false' otherwise.
*/
template <typename T> requires SentinelEnum<T>
inline constexpr bool operator!(T type) { return IsSentinel(type); }

/**
* @brief Checks if a given enum type is undefined (equal to the sentinel value). Enum type must be opted-in for this operator to work.
* 
* Equal to the operator! overload.
*
* @param type The enum type to check.
* @param [template] T The enum type.
* @param [requires] SentinelEnum<T> The enum type must be opted-in AND have T::ENUM_SENTINEL_VALUE for this operator to work.
*
* @return 'true' if the enum type is undefined (the sentinel value), 'false' otherwise.
*/
template <typename T> requires SentinelEnum<T>
inline constexpr bool IsSentinel(T type) { return type == T::ENUM_SENTINEL_VALUE; }

/**
* @brief Checks if a given enum type is defined (NOT equal to the sentinel value). Enum type must be opted-in for this operator to work.
*
* @param type The enum type to check.
* @param [template] T The enum type.
* @param [requires] SentinelEnum<T> The enum type must be opted-in AND have T::ENUM_SENTINEL_VALUE for this operator to work.
* 
* @return 'true' if the enum type is defined (NOT the sentinel value), 'false' otherwise.
*/
template <typename T> requires SentinelEnum<T>
inline constexpr bool operator*(T type) { return !(!type); }

/**
* @brief Converts a COUNTABLE enum value to a 0-based array index.
*
* Identical to the 'ToIndex' function.
*
* @param type The enum value to convert.
* @param [template] T The enum type.
* @param [requires] CountableEnum<T> The enum type must be opted-in as countable AND have T::COUNT
*
* @return The 0-based array index corresponding to the enum value.
*
* @throws If T::ENUM_SENTINEL_VALUE is passed, the assertion will fail in debug mode. Will not throw in release mode, but the result will underflow to SIZE_T_MAX.
*/
template <typename T> requires CountableEnum<T>
inline constexpr size_t operator+(T type) noexcept { return ToIndex(type); }

/**
* @brief Converts a COUNTABLE enum value to a 0-based array index.
* 
* Identical to the operator+ overload.
* 
* @param type The enum value to convert.
* @param [template] T The enum type.
* @param [requires] CountableEnum<T> The enum type must be opted-in as countable AND have T::COUNT
* 
* @return The 0-based array index corresponding to the enum value.
* 
* @throws If T::ENUM_SENTINEL_VALUE is passed, the assertion will fail in debug mode. Will not throw in release mode, but the result will underflow to SIZE_T_MAX.
*/
template <typename T> requires CountableEnum<T>
inline constexpr size_t ToIndex(T type) noexcept
{
    if constexpr (SentinelEnum<T>)
    {
        assert(type != T::ENUM_SENTINEL_VALUE && "Attempted to convert ENUM_SENTINEL_VALUE sentinel to array index!");
        return static_cast<size_t>(type) - 1;
    }
    else
    {
        return static_cast<size_t>(type);
    }
}

/**
* @brief Converts a 0-based array index back to a countable enum value.
* 
* @param index The 0-based array index to convert.
* @param [template] T The enum type.
* @param [requires] CountableEnum<T> The enum type must be opted-in as countable AND have T::COUNT
* 
* @return The sentinel enum value corresponding to the 0-based array index.
*/
template <typename T> requires CountableEnum<T>
inline constexpr T FromIndex(size_t index) noexcept
{
    if constexpr (SentinelEnum<T>)
    {
        return static_cast<T>(index + 1);
    }
    else
    {
        return static_cast<T>(index);
    }
}

/**
* @brief Resets a sentinel enum value to the sentinel value (undefined).
*
* Identical to the 'Reset' function.
*
* @param type The enum value to convert. WILL BE MODIFIED TO T::ENUM_SENTINEL_VALUE.
* @param [template] T The enum type.
* @param [requires] SentinelEnum<T> The enum type must be opted-in AND have T::ENUM_SENTINEL_VALUE for this operator to work.
*/
template <typename T> requires SentinelEnum<T>
inline constexpr void operator~(T& type) noexcept { Reset(type); }

/**
* @brief Resets a sentinel enum value to the sentinel value (undefined).
*
* Identical to the operator~ overload.
*
* @param type The enum value to convert. WILL BE MODIFIED TO T::ENUM_SENTINEL_VALUE.
* @param [template] T The enum type.
* @param [requires] SentinelEnum<T> The enum type must be opted-in AND have T::ENUM_SENTINEL_VALUE for this operator to work..
*/
template <typename T> requires SentinelEnum<T>
inline constexpr void Reset(T& type) noexcept { type = T::ENUM_SENTINEL_VALUE; }

/**
* @brief Returns the sentinel value of a sentinel enum type.
* 
* @param [template] T The enum type.
* @param [requires] SentinelEnum<T> The enum type must be opted-in AND have T::ENUM_SENTINEL_VALUE for this operator to work.
* 
* @return The sentinel value of the enum type.
*/
template <typename T> requires SentinelEnum<T>
inline constexpr T GetSentinel() noexcept { return T::ENUM_SENTINEL_VALUE; }

/**
* @brief Returns the number of valid enum values (excluding the sentinel value).
* 
* @param [template] T The enum type.
* @param [requires] SentinelEnumWithCount<T> The enum type must be opted-in AND have T::ENUM_SENTINEL_VALUE AND T::COUNT for this operator to work.
*/
template <typename T> requires CountableEnum<T>
inline constexpr size_t enumCount = []() {
    if constexpr (SentinelEnum<T>) {
        return static_cast<size_t>(T::COUNT) - 1;
    }
    else {
        return static_cast<size_t>(T::COUNT);
    }
}();

// Combining X-macros with the absurdity that is C++20 concepts. What a time to be alive.

#endif // ENUMMISCS_H_