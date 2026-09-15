#pragma once

#ifndef UITEXTSTYLEELEMENT_H_
#define UITEXTSTYLEELEMENT_H_

#include <SDL3/SDL_pixels.h>
#include <string>

#include "UITextAlignment.h"

struct TTF_Font;

/**
* @brief Abstract class for the style of text.
*/
class UITextStyleElement
{
protected:
	TTF_Font* textFont = nullptr;			/// Font of the text. NON OWNING!
	float originalFontSize = 0.0f;			/// Original font size of the text font.
	size_t currentFontHeight = 0;			/// Current font HEIGHT of the text font.
	size_t originalFontHeight = 0;			/// Original font HEIGHT of the text font.
	std::string fontFamily = "";			/// Font family name of the text font.
	SDL_Color textColor = { 0, 0, 0, 255 };	/// Color of the text.
	UITextAlignment textAlignment = { HorizontalAlignment::Center, VerticalAlignment::Middle };	/// Text alignment.

public:

	/**
	* @brief Constructor
	* 
	* @param textFont Font of the text.
	* @param textColor Color of the text.
	* @param textAlignment Alignment of the text.
	*/
	UITextStyleElement(
		TTF_Font* textFont = nullptr,
		const SDL_Color& textColor = { 0, 0, 0, 255 },
		const UITextAlignment& textAlignment = { HorizontalAlignment::Center, VerticalAlignment::Middle },
		const std::string& fontFamily = ""
	);

	/**
	* @brief Default destructor.
	*/
	virtual ~UITextStyleElement() = default;

	/**
	* @brief Sets the font of the text.
	*
	* This assumes that the font is from the same family. Use 'OverrideTextFont' if the font is from a different family / you want the 'original' size to be updated.
	* 
	* @param newFont New font for the text.
	*/
	virtual void SetTextFont(TTF_Font* newFont);

	/**
	* @brief Overrides the font of the text and updates the original font size.
	* 
	* Use 'SetTextFont' if the font is from the same family and you want to keep the original font size.
	* 
	* @param newFont New font for the text.
	* @param newFontFamily New font family name for the text.
	*/
	virtual void OverrideTextFont(TTF_Font* newFont, const std::string& newFontFamily);

	/**
	* @brief Returns the font of the text.
	*
	* @return TTF_Font pointer to the font of the text.
	*/
	virtual TTF_Font* GetTextFont() const;

	/**
	* @brief Returns the font family name of the text font.
	* 
	* @return A string containing the font family name of the text font.
	*/
	virtual std::string GetFontFamily() const;

	/**
	* @brief Sets the text alignment of the element.
	*
	* @param newAlignment The new alignment of the text.
	*/
	virtual void SetTextAlignment(const UITextAlignment& newAlignment);

	/**
	* @brief Returns the text alignment of the element.
	*
	* @return The text alignment of the element.
	*/
	virtual UITextAlignment GetTextAlignment() const;

	/**
	* @brief Sets the text color.
	*
	* @param color New text color.
	*/
	virtual void SetTextColor(const SDL_Color& newColor);

	/**
	* @brief Returns the text color.
	*
	* @return The text color.
	*/
	virtual SDL_Color GetTextColor() const;

	/**
	* @brief Returns the original font size (points) of the current font.
	* 
	* @return The original font size of the original font.
	*/
	virtual float GetOriginalFontSize() const;
	
	/**
	* @brief Returns the current font height (pixels) of the current font.
	* 
	* @return The current font height of the current font.
	*/
	virtual size_t GetCurrentFontHeight() const;

	/**
	* @brief Returns the original font height (pixels) of the current font.
	*
	* @return The original font height of the current font.
	*/
	virtual size_t GetOriginalFontHeight() const;
};

#endif // UITEXTSTYLEELEMENT_H_
