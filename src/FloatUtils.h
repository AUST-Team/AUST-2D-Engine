#pragma once

#ifndef FLOATUTILS_H_
#define FLOATUTILS_H_

#include <cmath>

#include "ConstantConfiguration.h"

/**
* @brief Contains helper functions for floats.
*/
namespace FloatUtils
{
	/**
	* @brief Tests if a float value is (near) zero with a given precision.
	*
	* @param value Float to test.
	* @param epsilon The precision of the calculation.
	*
	* @return 'true' if the value is (near) zero with the given precision, 'false' if otherwise.
	*/
	inline bool IsZeroF(float value, float epsilon = ConstantConfiguration::epsilon) 
	{ 
		return std::abs(value) < epsilon; 
	}

	/**
	* @brief Tests if two float values are (near) equal with a given precision.
	*
	* @param first First float value.
	* @param second Second float value.
	* @param epsilon The precision of the calculation.
	*
	* @return 'true' if the value are (near) equal with the given precision, 'false' if otherwise.
	*/
	inline bool IsEqualF(float first, float second, float epsilon = ConstantConfiguration::epsilon) 
	{ 
		return std::abs(first - second) < epsilon; 
	}

	/**
	* @brief Tests if the absolute values of two float values are (near) equal with a given precision.
	*
	* @param first First float value.
	* @param second Second float value.
	* @param epsilon The precision of the calculation.
	*
	* @return 'true' if the absolute values are (near) equal with the given precision, 'false' if otherwise.
	*/
	inline bool IsEqualAbsF(float first, float second, float epsilon = ConstantConfiguration::epsilon) 
	{
		return IsEqualF(std::abs(first), std::abs(second), epsilon);
	}

	/**
	* @brief Tests if the first float is (strictly) greater than the second with a given precision.
	*
	* @param first First float value.
	* @param second Second float value.
	* @param epsilon The precision of the calculation.
	*
	* @return 'true' if the first value is greater than the second by at least the given precision, 'false' if otherwise.
	*/
	constexpr inline bool IsGreaterF(float first, float second, float epsilon = ConstantConfiguration::epsilon) 
	{ 
		return (first - second) > epsilon; 
	}

	/**
	 * @brief Tests if the absolute value of the first float is (strictly) greater than the absolute value of the second float with a given precision.
	 *
	 * @param first First float value.
	 * @param second Second float value.
	 * @param epsilon Optional precision bias to avoid floating-point edge overlap.
	 *
	 * @return 'true' if first value's absolute value is greater than the second value's absolute value by atleast the given precision, 'false' otherwise.
	 */
	inline bool IsGreaterAbsF(float first, float second, float epsilon = ConstantConfiguration::epsilon) 
	{ 
		return IsGreaterF(std::abs(first), std::abs(second), epsilon); 
	}

	/**
	* @brief Tests if the first float is greater than or (near) equal to the second with a given precision.
	*
	* @param first First float value.
	* @param second Second float value.
	* @param epsilon The precision of the calculation.
	*
	* @return 'true' if the first value is greater than or (near) equal to the second with the given precision, 'false' if otherwise.
	*/
	constexpr inline bool IsGreaterOrEqualF(float first, float second, float epsilon = ConstantConfiguration::epsilon) 
	{ 
		return IsGreaterF(first, second, epsilon) || IsEqualF(first, second, epsilon); 
	}

	/**
	* @brief Tests if the absolute value of the first float is greater than or (near) equal to the absolute value of the second float with a given precision.
	*
	* @param first First float value.
	* @param second Second float value.
	* @param epsilon The precision of the calculation.
	*
	* @return 'true' if first value's absolute value is greater than or (near) equal to the second value's absolute value with the given precision, 'false' otherwise.
	*/
	inline bool IsGreaterEqualAbsF(float first, float second, float epsilon = ConstantConfiguration::epsilon) 
	{ 
		return IsGreaterAbsF(first, second, epsilon) || IsEqualAbsF(first, second, epsilon); 
	}

	/**
	* @brief Tests if the first float is (strictly) less than the second with a given precision .
	*
	* @param first First float value.
	* @param second Second float value.
	* @param epsilon The precision of the calculation.
	*
	* @return 'true' if the first value is less than the second by at least the given precision, 'false' if otherwise.
	*/
	constexpr inline bool IsLessF(float first, float second, float epsilon = ConstantConfiguration::epsilon) 
	{ 
		return (second - first) > epsilon; 
	}

	/**
	 * @brief Tests if the absolute value of the first float is (strictly) less than the second with a given precision.
	 *
	 * @param first First float value.
	 * @param second Second float value.
	 * @param epsilon Optional precision bias to avoid floating-point edge overlap.
	 *
	 * @return 'true' if first value's absolute value is less than the second value's absolute value by atleast the given precision, 'false' otherwise.
	 */
	inline bool IsLessAbsF(float first, float second, float epsilon = ConstantConfiguration::epsilon) 
	{ 
		return IsLessF(std::abs(first), std::abs(second), epsilon);
	}

	/**
	* @brief Tests if the first float is less than or nearly equal to the second, with a given precision.
	*
	* @param first First float value.
	* @param second Second float value.
	* @param epsilon The precision of the calculation.
	*
	* @return 'true' if the first value is less than or (near) equal to the second with the given precision, 'false' if otherwise.
	*/
	constexpr inline bool IsLessOrEqualF(float first, float second, float epsilon = ConstantConfiguration::epsilon)
	{
		return IsLessF(first, second, epsilon) || IsEqualF(first, second, epsilon);
	}

	/**
	* @brief Tests if the absolute value of the first float is less than or (near) equal to the absolute value of the second float with a given precision.
	*
	* @param first First float value.
	* @param second Second float value.
	* @param epsilon The precision of the calculation.
	*
	* @return 'true' if the first value's absolute value is less than or (near) equal to the second value's absolute value with the given precision, 'false' if otherwise.
	*/
	inline bool IsLessEqualAbsF(float first, float second, float epsilon = ConstantConfiguration::epsilon)
	{
		return IsLessAbsF(first, second, epsilon) || IsEqualAbsF(first, second, epsilon);
	}

	/**
	* @brief Clamps a float value between a minimum and maximum bound.
	*
	* @param value Float value to clamp.
	* @param min The minimum bound.
	* @param max The maximum bound.
	*
	* @return The clamped float value within the given bounds.
	*/
	constexpr inline float ClampF(float value, float min, float max)
	{
		return (value < min) ? min : (value > max) ? max : value;
	}
}

#endif // FLOATUTILS_H_