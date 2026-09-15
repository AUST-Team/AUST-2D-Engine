#pragma once

#ifndef FNV1AHASH_H_
#define FNV1AHASH_H_

#include <cstdint>
#include <string>

#include "ConstantConfiguration.h"

/**
* @brief Namespace for hashing strings.
* 
* Totally didn't chose FNV because FALLOUT NEW VEGAS!
*/
namespace FNV1aHash 
{
    /**
    * @brief Hashes a string using the 64-bit FNV-1a algorithm.
    * 
    * @param str String to be hashed.
    * 
    * @return The hash if the string. 0 is reserved, will return 1 if the hash is 0.
    */
    constexpr inline HashID HashString(const char* str)
    {
        HashID hash = ConstantConfiguration::fnvOffsetBasis64;
        while (*str)
        {
            hash ^= static_cast<HashID>(*str++);
            hash *= ConstantConfiguration::fnvPrime64;
        }
        return hash == 0 ? 1 : hash;
    }

    /**
    * @brief Hashes a string using the 64-bit FNV-1a algorithm.
    *
    * @param s String to be hashed.
    *
    * @return The hash if the string. 0 is reserved, will return 1 if the hash is 0.
    */
    constexpr inline HashID HashString(const std::string& s)
    {
        HashID hash = ConstantConfiguration::fnvOffsetBasis64;
        for (char c : s)
        {
            hash ^= static_cast<HashID>(c);
            hash *= ConstantConfiguration::fnvPrime64;
        }
        return hash == 0 ? 1 : hash;
    }
}

#endif // FNV1AHASH_H_