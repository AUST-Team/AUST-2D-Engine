#pragma once

#ifndef ARRAYMISCS_H_
#define ARRAYMISCS_H_

#include <array>
#include <utility>

/**
* @brief Namespace for array related functions.
*/
namespace ArrayMiscs
{
    /**
    * @brief Makes an array be filled with a given value.
    *
    * @param val Value to fill the array with.
    * @param Is Index sequence.
    * @param [template] T Type of the value / array.
    * @param [template] N Size of the array.
    * @param [template] Is Index sequence of the array (list of values [0 to N-1]).
    *
    * @return An array of type T and size N, with all elements equal to 'val'
    *
    template <typename T, size_t N, size_t... Is>
    static constexpr std::array<T, N> MakeFilledArrayImplementation(const T& val, std::index_sequence<Is...>)
    {
        // Ok so.
        // ... -> expands the (Is, val) for each index in the sequence.
        // Is -> a compile time index sequence, so basically a list of [0 to N-1]. Think of it as a compile time 'for' loop.
        // (void) cast the value of the index to void to tell the compiler we don't use the value on purpose.
        // (Is, val) -> Comma operator evaluates both expressions, and returns the last expression (left one is discarded).
        // Isn't C++ code very readable, and wonderful? All this for compile time evaluation.
        return { ((void)Is, val)... };
    }
    */

    /**
    * @brief Makes an array be filled with a given value.
    *
    * @param val Value to fill the array with.
    * @param Is Index sequence.
    * @param [template] T Type of the value / array.
    * @param [template] N Size of the array.
    *
    * @return An array of type T and size N, with all elements equal to 'val'
    */
    template <typename T, size_t N>
    inline constexpr std::array<T, N> MakeFilledArray(const T& val)
    {
        //// Did you know you can make templates with lambdas? Now you know!
        //return [&]<size_t... Is>(std::index_sequence<Is...>) {
        //    return std::array<T, N>{ ((void)Is, val)... };
        //}(std::make_index_sequence<N>{}); // See above for explanations.

        std::array<T, N> arr{};
        arr.fill(val); // std::array::fill is fully constexpr in C++20! You learn something new everyday.
        return arr;
    }
};

#endif // ARRAYMISCS_H_