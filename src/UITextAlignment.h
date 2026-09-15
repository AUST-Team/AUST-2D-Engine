#pragma once

#ifndef UITEXTALIGNMENT_H_
#define UITEXTALIGNMENT_H_

#include <SDL3/SDL_rect.h>
#include <string>

#include "EnumMiscs.h"

#define VERTICAL_ALIGNMENT_LIST(X)  \
    ENUM_SENTINEL_X_MACRO           \
    X(Top,          1,  "top")      \
    X(Middle,       2,  "middle")   \
    X(Bottom,       3,  "bottom")                

#define HORIZONTAL_ALIGNMENT_LIST(X)    \
    ENUM_SENTINEL_X_MACRO               \
    X(Left,         1,  "left")         \
    X(Center,       2,  "center")       \
    X(Right,        3,  "right")    

/**
* @brief Enumeration of the types of vertical alignment for an UI text element.
*/
enum class VerticalAlignment : SentinelEnumType
{
    /*
	ENUM_SENTINEL_VALUE,  /// Undefined vertical alignment.
    Top,        /// Top vertical alignment.
    Middle,     /// Middle vertical alignment.
    Bottom,     /// Bottom vertical alignment.
    */

#define X(name, val, str) name = val,
    VERTICAL_ALIGNMENT_LIST(X)
#undef X
    COUNT   /// Number of elements.
};

/**
* @brief Enumeration of the types of horizontal alingment for an UI text element.
*/
enum class HorizontalAlignment : SentinelEnumType
{
    /*
    ENUM_SENTINEL_VALUE,  /// Undefined horizontal alignment.
    Left,       /// Left horizontal aligmnment.
    Center,     /// Center horizontal alignment.
    Right,      /// Right horizontal alignment.
    */
#define X(name, val, str) name = val,
    HORIZONTAL_ALIGNMENT_LIST(X)
#undef X
    COUNT   /// Number of elements.
};

template <>
inline constexpr bool isSentinelEnum<VerticalAlignment> = true;

template <>
inline constexpr bool isCountableEnum<VerticalAlignment> = true;

template <>
inline constexpr bool isSentinelEnum<HorizontalAlignment> = true;

template <>
inline constexpr bool isCountableEnum<HorizontalAlignment> = true;

/**
* @brief Structure for the alignment of an UI text element.
*/
struct UITextAlignment
{
    HorizontalAlignment horizontalAlignment;    /// Horizontal alingment of the text.
    VerticalAlignment verticalAlignment;        /// Vertical alignment of the text.

    /**
    * @brief Is-equal operator.
    */
    bool operator==(const UITextAlignment& other) const;

    /**
    * @brief Is-not-equal operator.
    */
    bool operator!=(const UITextAlignment& other) const;
};

/**
* @brief Returns the aligned text position.
*
* @param bounds Bounds of the element.
* @param textWidth The width of the text.
* @param textHeight The height of the text.
* @param alignment Alignment of the text.
* @param totalBlockHeight The total height of the text block. Equal to the text height if the text is one line.
* @param padding The padding around the edges.
*
* @return An SDL_FPoint containing the aligned X and Y coordinates.
*/
SDL_FPoint GetAlignedPosition(const SDL_FRect& bounds, float textWidth, float textHeight, const UITextAlignment& alignment, float totalBlockHeight = -1.0f, float padding = 0.0f);

/**
 * @brief Returns the vertical alignment mode from a string.
 * 
 * @param vertAlignment String containing the vertical alignment.
 * 
 * @return The VerticalAlignment according to the string, or VerticalAlignment::ENUM_SENTINEL_VALUE if the string doesn't match anything.
 */
VerticalAlignment VerticalAlignmentFromString(const std::string& vertAlignment);

/**
 * @brief Returns the name of the vertical alignment mode.
 * 
 * @param vertAlignment The vertical alignment mode.
 * 
 * @return The string containing the name of the vertical alignment, or "[ConstantConfiguration::enumSentinelStringLower]" if the mode is unknown.
 */
std::string VerticalAlignmentToString(VerticalAlignment vertAlignment);

/**
 * @brief Returns the horizontal alignment mode from a string.
 * 
 * @param horizAlignment String containing the horizontal alignment.
 * 
 * @return The HorizontalAlignment according to the string, or HorizontalAlignment::ENUM_SENTINEL_VALUE if the string doesn't match anything.
 */
HorizontalAlignment HorizontalAlignmentFromString(const std::string& horizAlignment);

/**
 * @brief Returns the name of the horizontal alignment mode.
 * 
 * @param horizAlignment The horizontal alignment mode.
 * 
 * @return The string containing the name of the horizontal alignment, or "[ConstantConfiguration::enumSentinelStringLower]" if the mode is unknown.
 */
std::string HorizontalAlignmentToString(HorizontalAlignment horizAlignment);


#endif // UITEXTALIGNMENT_H_