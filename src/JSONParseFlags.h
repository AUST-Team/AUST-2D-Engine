#pragma once

#ifndef JSONPARSEFLAGS_H_
#define JSONPARSEFLAGS_H_

#include <cstdint>
#include <type_traits>

/**
* @brief Flags for parsing JSON files.
*/
enum class JSONParseFlags : uint8_t
{
    None            = 0,        /// No flags set.
	ParseDefines	= 1 << 0,	/// Flag if the JSON should have definitions parsed.
	DeleteDefines	= 1 << 1	/// Flag if the JSON should have the definitions deleted after being parsed. Has no effect if defines are not also parsed.
};

/// The type of the MapUpdateFlags enum.
using JSONParseFlagsType = std::underlying_type_t<JSONParseFlags>;

/**
* @brief Bitwise OR operator.
*/
constexpr inline JSONParseFlags operator|(JSONParseFlags a, JSONParseFlags b)
{
    return static_cast<JSONParseFlags>(
        static_cast<JSONParseFlagsType>(a) |
        static_cast<JSONParseFlagsType>(b)
    );
}

/**
* @brief Assign bitwise OR operator.
*/
constexpr inline JSONParseFlags& operator|=(JSONParseFlags& a, JSONParseFlags b) { return a = a | b; }

/**
* @brief Bitwise AND operator.
*/
constexpr inline JSONParseFlags operator&(JSONParseFlags a, JSONParseFlags b)
{
    return static_cast<JSONParseFlags>(
        static_cast<JSONParseFlagsType>(a) &
        static_cast<JSONParseFlagsType>(b)
        );
}

/**
* @brief Assign bitwise AND operator.
*/
constexpr inline JSONParseFlags& operator&=(JSONParseFlags& a, JSONParseFlags b) { return a = a & b; }

/**
* @brief Bitwise NOT operator.
*/
constexpr inline JSONParseFlags operator~(JSONParseFlags f)
{
    return static_cast<JSONParseFlags>(~static_cast<JSONParseFlagsType>(f));
}

/**
 * @brief Checks whether one or more flags are set in a JSONParseFlags value.
 *
 * @param flags The combined set of flags to test.
 * @param test  The flag or flags to check for.
 *
 * @return 'true' if at least one of the requested flags is set in 'flags', 'false' otherwise.
 */
constexpr inline bool HasFlag(JSONParseFlags flags, JSONParseFlags test) { return (flags & test) != JSONParseFlags::None; }

#endif // JSONPARSEFLAGS_H_