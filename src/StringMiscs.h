#pragma once

#ifndef STRINGMISCS_H_
#define STRINGMISCS_H_

#include <string>
#include <vector>

#include "ConstantConfiguration.h"

struct SDL_FRect;
struct TTF_Font;

namespace StringMiscs
{
	/**
	* @brief Transforms a string to all lowercase.
	*
	* Because god forbid C++ have an inbuilt method / function for this really common thing.
	*
	* @param str Original string. Note that it will not be modified.
	*
	* @return A string containing the original string's characters but all lowercase.
	*/
	std::string ToLower(std::string str);

	/**
	* @brief Capitalizes the first letter of a string.
	*
	* @param str Original string. NOT MODIFIED.
	*
	* @return A string containing the original string's characters, but the first letter is capitalized.
	*/
	std::string CapitalizeFirst(std::string str);

	/**
	* @brief Checks if a string is the sentinel string for an enumeration.
	* 
	* Enum type does not matter, as the sentinel string is the same for all enums.
	* 
	* @param str The string to check.
	* @param sentinelStr The sentinel string to compare against. Defaults to ConstantConfiguration::enumSentinelStringLower.
	* 
	* @return True if the string is the sentinel string, false otherwise.
	*/
	bool IsStringEnumSentinel(const std::string& str, const std::string& sentinelStr = ConstantConfiguration::enumSentinelStringLower);

	/**
	* @brief Wraps the text on the screen.
	*
	* @param text String to be wrapped.
	* @param bounds The dimensions of the rectangle to be wrapped around.
	* @param font The font used.
	* @param padding The padding around the hoziontal edges.
	*
	* @return A vector containing the text broken up in multiple lines.
	*/
	std::vector<std::string> WrapText(const std::string& fullText, const SDL_FRect& bounds, TTF_Font* font, float padding = 40.0f);

	/**
	* @brief Truncates a text to fit the maximum width provided.
	*
	* @param fullText The full text to truncate.
	* @param font Font of the text.
	* @param maxWidth Maximum width allowed.
	*/
	std::string TruncateTextToFit(const std::string& fullText, TTF_Font* font, int maxWidth);
}

#endif // STRINGMISCS_H_