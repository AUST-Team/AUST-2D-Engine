#pragma once

#ifndef JSONPARSER_H_
#define JSONPARSER_H_

#include <nlohmann/json.hpp>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <string>
#include <vector>
#include <filesystem>

#include "JSONParseFlags.h"
#include "SystemAction.h"
#include "UITextAlignment.h"
#include "UITextStyleElement.h"
#include "UIElementProperties.h"
#include "Size.h"
#include "DialogueData.h"
#include "AudioCommand.h"
#include "ScoreEntry.h"
#include "AudioClip.h"

struct MIX_Mixer;

namespace JSONParser
{
	/**
	* @brief Parses JSON defines and string-substitutes them.
	*
	* Can be safely called if there's no defines in the file. It will NOT parse the entire file if there's no defines.
	*
	* @param [out] root The root JSON object containing the defines and the JSON to be parsed.
	* @param shouldDeleteDefines Flag if the defines should be deleted after parsing.
	*/
	void ParseJSONDefines(nlohmann::json& root, JSONParseFlags shouldDeleteDefines = JSONParseFlags::DeleteDefines);

	/**
	* @brief Returns the (SDL) color from a JSON array / object / string.
	*
	* @param colorJSON The JSON array of the colors [R, G, B, Opacity],
	* or the JSON object of {"r": 0-255, "g": 0-255, "b": 0-255, "a": 0-255},
	* or the JSON string of "0xFFF", "#00FF00", "ABCDEFh"
	* @param def Reference to the 'default' color.
	* 
	* @return The parsed color, the 'def' if color cannot be parsed.
	*/
	SDL_Color ParseJSONColor(const nlohmann::json& colorJSON, const SDL_Color& def = { 255, 255, 255, 255 });

	/**
	* @brief Parses a JSON and returns the vector of SystemActions.
	*
	* @param eventJSON The JSON containing the action data, either object or array.
	*
	* @return A vector of SystemAction objects parsed from the JSON, or an empty vector if no valid actions were found.
	*/
	std::vector<SystemAction> ParseJSONSystemActions(const nlohmann::json& eventJSON);

	/**
	* @brief Returns the text alignment from a JSON.
	*
	* @param alignmentJSON JSON containing the alignment object { "horizontal": "center", "vertical": "middle" }.
	* @param def Reference to the default alignment.
	* 
	* @return The prased text alignment, or 'def' if alignment cannot be parsed.
	*/
	UITextAlignment ParseJSONAlignment(const nlohmann::json& alignmentJSON, const UITextAlignment& def = { HorizontalAlignment::Center, VerticalAlignment::Middle });

	/**
	* @brief Parses and returns an UITextStyleElement.
	* 
	* @param styleJSON JSON containing the text style.
	* @param def Reference to the default text style.
	* 
	* @return The parsed text style element, or 'def' if the style cannot be parsed.
	*/
	UITextStyleElement ParseJSONTextStyle(const nlohmann::json& styleJSON, const UITextStyleElement& def = UITextStyleElement());

	/**
	* @brief Parses and returns UIElement properties.
	* 
	* @param elementJSON JSON containing the UIElement properties.
	* @param def Reference to the default element properties object.
	* 
	* @return The parsed element properties, or 'def' if the properties cannot be parsed.
	*/
	UIElementProperties ParseJSONUIElementProps(const nlohmann::json& elementJSON, const UIElementProperties& def = UIElementProperties{});

	/**
	* @brief Parses an UITable's datasource and return the matrix of strings.
	* 
	* @param sourceJSON The JSON object containing the datasource for the table.
	* @param def Reference to the default datasource.
	* 
	* @return The matrix of strings from the datasource, or 'def' if datasource is unknown.
	*/
	std::vector<std::vector<std::string>> ParseJSONTableDatasource(const nlohmann::json& sourceJSON, const std::vector<std::vector<std::string>>& def = {});

	/**
	* @brief Parses padding from JSON to SDL_FPoint.
	* 
	* @param paddingJSON The JSON containing the padding.
	* @param def Reference to the default padding structure.
	* 
	* @return The parsed padding, of 'def' if it cannot be parsed.
	*/
	SDL_FPoint ParseJSONPadding(const nlohmann::json& paddingJSON, const SDL_FPoint& def = { 0.0f, 0.0f });

	/**
	* @brief Parses a JSON path and returns the OS-dependant path.
	*
	* @param pathJSON JSON containing the path (string).
	* @param def Default path.
	*
	* @return The parsed path, or 'def' if path cannot be parsed.
	*/
	std::filesystem::path ParseJSONPath(const nlohmann::json& pathJSON, const std::filesystem::path& def = std::filesystem::path());

	/**
	* @brief Parses dialogue data from a JSON.
	* 
	* @param dataJSON JSON containing the dialogue data.
	* @param def Default data if JSON cannot be parsed.
	* 
	* @return The parsed data, or "def" if it cannot be parsed.
	*/
	DialogueData ParseJSONDialogueData(const nlohmann::json& dataJSON, const DialogueData& def = DialogueData{});

	/**
	* @brief Parses an audio command from a JSON.
	* 
	* @param audioJSON JSON containing the audio command.
	* @param def Default command if JSON cannot be parsed.
	* 
	* @return The parsed command, or 'def' if it cannot be parsed.
	*/
	AudioCommand ParseJSONAudioCommand(const nlohmann::json& audioJSON, const AudioCommand& def = AudioCommand{});

	/**
	* @brief Parses a score entry from a JSON.
	* 
	* @param scoreJSON JSON containing the score entry.
	* @param def Default score entry if JSON cannot be parsed.
	* 
	* @return The parsed score entry, or 'def' if it cannot be parsed.
	*/
	ScoreEntry ParseJSONScoreEntry(const nlohmann::json& scoreJSON, const ScoreEntry& def = ScoreEntry{});

	/**
	* @brief Parses audio playback options from a JSON.
	* 
	* @param playbackJSON JSON containing the audio playback options.
	* @param def Default audio playback options if JSON cannot be parsed.
	* 
	* @return The parsed audio playback options, or 'def' if it cannot be parsed.
	*/
	AudioPlaybackOptions ParseJSONAudioPlaybackOptions(const nlohmann::json& playbackJSON, const AudioPlaybackOptions& def = AudioPlaybackOptions{});

	/**
	* @brief Returns the (SDL) floating point rectangle from a JSON array / object
	*
	* @param rectJSON The JSON array of the rectangle [x, y, w, h],
	* or the JSON object of {"x": X, "y": Y, "w": W, "height": H},
	* @param def Reference to the 'default' rectangle.
	*
	* @return The parsed rectangle, or 'def' if rectangle cannot be parsed.
	*/
	template<typename T>
	concept ValidRectType = std::same_as<T, SDL_FRect> || std::same_as<T, SDL_Rect>;
	template <ValidRectType T = SDL_FRect>
	T ParseJSONRect(const nlohmann::json& rectJSON, const T& def = T{})
	{
		/*
			What this function expects:

			Note that rectJSON is the default JSON object / array:

			[100, 200, 30, 40]

			{"x": 10, "y": 200, "w": 10, "height": 20 }
		*/

		using ElementType = decltype(T::x);

		if (rectJSON.is_array() && rectJSON.size() == 4)
		{
			if (!rectJSON[0].is_number() || !rectJSON[1].is_number() ||
				!rectJSON[2].is_number() || !rectJSON[3].is_number())
			{
				SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONRect: Rect JSON array contains non-numeric values.");
				return def;
			}

			return SDL_FRect {
				rectJSON.at(0).get<ElementType>(),
				rectJSON.at(1).get<ElementType>(),
				rectJSON.at(2).get<ElementType>(),
				rectJSON.at(3).get<ElementType>()
			};
		}
		else if (rectJSON.is_object())
		{
			std::string xKey = rectJSON.contains("x") ? "x" : "";
			std::string yKey = rectJSON.contains("y") ? "y" : "";
			std::string wKey = rectJSON.contains("w") ? "w" : (rectJSON.contains("width") ? "width" : "");
			std::string hKey = rectJSON.contains("h") ? "h" : (rectJSON.contains("height") ? "height" : "");

			if (xKey.empty() || yKey.empty() || wKey.empty() || hKey.empty() ||
				!rectJSON[xKey].is_number() || !rectJSON[yKey].is_number() ||
				!rectJSON[wKey].is_number() || !rectJSON[hKey].is_number())
			{
				SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONRect: Rect JSON object missing required x, y, w/width, or h/height numeric fields.");
				return def;
			}

			return SDL_FRect {
				rectJSON.at(xKey).get<ElementType>(),
				rectJSON.at(yKey).get<ElementType>(),
				rectJSON.at(wKey).get<ElementType>(),
				rectJSON.at(hKey).get<ElementType>()
			};
		}

		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONRect: Rect JSON is not an object or a 4-element array.");
		return def;
	}

	/**
	* @brief Parses an point from a JSON.
	* 
	* @param pointJSON JSON containing the point data.
	* @param def Default value to return in case point cannot be parsed.
	* @param [template] T Type of point (SDL_Point / FPoint)
	* 
	* @return The parsed point, or 'def' if it cannot be parsed.
	*/
	template<typename T>
	concept ValidPointType = std::same_as<T, SDL_FPoint> || std::same_as<T, SDL_Point>;
	template <ValidPointType T = SDL_FPoint>
	T ParseJSONPoint(const nlohmann::json& pointJSON, const T& def = T{})
	{
		/**
			What this function expects:
			
			"value": 40
			"value": [23.5, 66.7]
			"value": { "x": 10, "y": 0 }
		*/

		// Get the type (int for SDL_Point, float for SDL_FPoint).
		using ElementType = decltype(T::x);

		// Single value -> {v, v}
		if (pointJSON.is_number())
		{
			ElementType val = pointJSON.get<ElementType>();
			return T { .x = val, .y = val };
		}
		// Array -> {x, y}
		else if (pointJSON.is_array() && pointJSON.size() >= 2)
		{
			if (pointJSON[0].is_number() && pointJSON[1].is_number())
			{
				return T {
					.x = pointJSON.at(0).get<ElementType>(),
					.y = pointJSON.at(1).get<ElementType>()
				};
			}
		}
		// Object -> {x, y}
		else if (pointJSON.is_object())
		{
			T result = def;

			if (pointJSON.contains("x") && pointJSON["x"].is_number())
			{
				result.x = pointJSON.at("x").get<ElementType>();
			}

			if (pointJSON.contains("y") && pointJSON["y"].is_number())
			{
				result.y = pointJSON.at("y").get<ElementType>();
			}

			return result;
		}

		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONPoint: Given JSON is not a number, array with atleast 2 elements, or object. Defaulting to \"def\"");
		return def;
	}

	/**
	* @brief Parses a JSON object and returns the time.
	*
	* @param timeJSON JSON object containing the value and measurement unit. Should the JSON be just a number, it will be treated as miliseconds.
	* @param def Default value to return if no valid time can be parsed.
	* @param [template] T The type of the time to return. Can be float, double, uint64_t or int.
	*
	* @return The parsed time, as miliseconds, or 'def' if there's no valid element to parse.
	*/
	template<typename T>
	concept ValidTimeType = std::same_as<T, float> ||
		std::same_as<T, double> ||
		std::same_as<T, uint64_t> ||
		std::same_as<T, int64_t>;
	template <ValidTimeType T = float>
	T ParseJSONTime(const nlohmann::json& timeJSON, T def = T{})
	{
		/*
			What this function expects:

			"value": 1.5
			"value": 10
			{ "ms": 100 }
			{ "s": 2 }
			{ "seconds": 0.5 }
		*/

		double milliseconds = 0.0;
		bool parsedSuccessfully = false;

		if (timeJSON.is_number())
		{
			// Raw input -> seconds.
			milliseconds = timeJSON.get<double>() * 1000.0;
			parsedSuccessfully = true;
		}
		else if (timeJSON.is_object())
		{
			if (timeJSON.contains("seconds") && timeJSON["seconds"].is_number())
			{
				milliseconds = timeJSON.at("seconds").get<double>() * 1000.0;
				parsedSuccessfully = true;
			}
			else if (timeJSON.contains("s") && timeJSON["s"].is_number())
			{
				milliseconds = timeJSON.at("s").get<double>() * 1000.0;
				parsedSuccessfully = true;
			}
			else if (timeJSON.contains("ms") && timeJSON["ms"].is_number())
			{
				milliseconds = timeJSON.at("ms").get<double>();
				parsedSuccessfully = true;
			}
		}

		if (parsedSuccessfully)
		{
			return static_cast<T>(milliseconds);
		}

		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONTime: Invalid time format in JSON. Returning default.");
		return def;
	}

	/**
	* @brief Parses a JSON object / array / number and returns the 'Size' of it.
	*
	* @param sizeJSON JSON object / array / number containing the two values for the width and height.
	* @param def Default value to return if no valid size can be parsed.
	* @param [template] T The type of the size to return. Can be Size, USize, FSize.
	*
	* @return The parsed size, or 'def' if it cannot be parsed.
	*/
	template<typename T>
	concept ValidSizeType = std::same_as<T, Size> ||
		std::same_as<T, USize> ||
		std::same_as<T, FSize>;
	template <ValidSizeType T = Size>
	T ParseJSONSize(const nlohmann::json& sizeJSON, T def = T{})
	{
		/*
			What this function expects:

			"value": { "w": 10, "h": 10 }
			"value": { "width": 10, "height": 10 }
			"value": 10
			"value": [10, 10]
		*/

		using ElementType = decltype(T::w);

		// Single value -> {v, v}
		if (sizeJSON.is_number())
		{
			ElementType val = sizeJSON.get<ElementType>();
			return T{ .w = val, .h = val };
		}
		// Array -> {w, h}
		else if (sizeJSON.is_array() && sizeJSON.size() >= 2)
		{
			if (sizeJSON[0].is_number() && sizeJSON[1].is_number())
			{
				return T{
					.w = sizeJSON.at(0).get<ElementType>(),
					.h = sizeJSON.at(1).get<ElementType>()
				};
			}
		}
		// Object -> {w, h}
		else if (sizeJSON.is_object())
		{
			T result = def;

			if (sizeJSON.contains("w") && sizeJSON["w"].is_number())
			{
				result.w = sizeJSON.at("w").get<ElementType>();
			}
			else if (sizeJSON.contains("width") && sizeJSON["width"].is_number())
			{
				result.w = sizeJSON.at("width").get<ElementType>();
			}

			if (sizeJSON.contains("h") && sizeJSON["h"].is_number())
			{
				result.h = sizeJSON.at("h").get<ElementType>();
			}
			else if (sizeJSON.contains("height") && sizeJSON["height"].is_number())
			{
				result.h = sizeJSON.at("height").get<ElementType>();
			}

			return result;
		}

		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "JSONParser.ParseJSONSize: Given JSON is not a number, array with at least 2 elements, or object. Defaulting to \"def\"");
		return def;
	}
}

#endif // JSONPARSER_H_