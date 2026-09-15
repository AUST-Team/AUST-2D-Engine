#pragma once

#ifndef UITEXTELEMENT_H_
#define UITEXTELEMENT_H_

#include <string>

#include "UITextStyleElement.h"


/**
* @brief Base class for a UI element that contains text.
*/
class UITextElement : public UITextStyleElement
{
protected:
	std::string text = "";	/// Text of the elemenet.
	std::string textEffectKey = "";	/// Key for the text effect to apply to the text. Empty if no effect.

public:

	/**
	* @brief Constructor.
	*
	* @param text Text of the element.
	* @param font The font of the element.
	* @param color Color of the text.
	* @param textAlignment Alignment of the text.
	* @param fontFamily Font family name of the text font.
	*/
	UITextElement(
		const std::string& text = "",
		TTF_Font* textFont = nullptr,
		const SDL_Color& textColor = { 255, 255, 255, 255 },
		const UITextAlignment& textAlignment = { HorizontalAlignment::Center, VerticalAlignment::Middle },
		const std::string& fontFamily = ""
	);

	/**
	* @brief Destructor.
	*/
	virtual ~UITextElement() = default;

	/**
	* @brief Sets a new text.
	*
	* @param newText New text to be displayed
	*/
	virtual void SetText(const std::string& newText);

	/**
	* @brief Returns the text of the element.
	*
	* @return A string containing the text of the element.
	*/
	virtual std::string GetText() const;

	/**
	* @brief Sets the key for the text effect to apply to the text. If the key is empty, no effect will be applied.
	* 
	* @param newEffectKey The new key for the text effect to apply to the text. If the key is empty, no effect will be applied.
	*/
	virtual void SetTextEffectKey(const std::string& newEffectKey);

	/**
	* @brief Returns the key for the text effect to apply to the text. If the key is empty, no effect will be applied.
	* 
	* @return A string containing the key for the text effect to apply to the text. If the key is empty, no effect will be applied.
	*/
	virtual std::string GetTextEffectKey() const;
};

#endif // UITEXTELEMENT_H_