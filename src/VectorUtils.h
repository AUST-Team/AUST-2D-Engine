#pragma once

#ifndef VECTORUTILS_H_
#define VECTORUTILS_H_

#include <corecrt_math_defines.h>

struct SDL_FPoint;

/**
* @brief Namespace that contains utility functions for vectors (the mathematical kind).
*/
namespace VectorUtils
{
    /**
    * @brief Calculates the dot product between two vectors.
    * 
    * Dot product is: 'x1 * x2 + y1 * y2'
    * 
    * @param a The first vector.
    * @param b The second vector.
    * 
    * @return The dot product between 'a' and 'b'.
    */
    constexpr inline float DotProduct(const SDL_FPoint& a, const SDL_FPoint& b)
    {
        return a.x * b.x + a.y * b.y;
    }

    /**
    * @brief Normalizes a vector.
    * 
	* Normalization is: 'v / |v|'. |v| is the length of the vector, calculated as 'sqrt(x^2 + y^2)'.
    * 
    * @param v Vector to normalize.
    * 
	* @return The normalized vector. A normalized vector has a length of 1 and points in the same direction as the original vector. If the input vector has a length of 0, returns a zero vector.
    */
    inline SDL_FPoint Normalize(const SDL_FPoint& v)
    {
        const float len = std::sqrt(v.x * v.x + v.y * v.y);
        if (len == 0)
        {
            return SDL_FPoint { 0.0f, 0.0f };
        }

        return SDL_FPoint { 
            .x = v.x / len, 
            .y = v.y / len 
        };
    }

    /**
    * @brief Calculates the dot product between two normalized vectors with an angle of 'degrees' value between them.
    * 
    * The dot product between two normalized vectors is: 'cos(angle_in_radians)'
    * 
    * @param degrees The degrees of the angle.
    * 
    * @return The dot product between the two normalized vectors.
    */
    inline float DegreesToDotProduct(float degrees)
    {
        return static_cast<float>(std::cos(degrees * M_PI / 180.0f));
    }
}

#endif // VECTORUTILS_H_
