#pragma once

#ifndef UICACHEDTEXTELEMENT_H_
#define UICACHEDTEXTELEMENT_H_

#include "UITextElement.h"
#include "UICachedTextureElement.h"

/**
* @brief Base class for UI elements that have text and use caching.
* 
* Use for elements that are mostly static.
*/
class UICachedTextElement : public UITextElement, public UICachedTextureElement
{
protected:

	// No copying allowed due to the cached texture.
	UICachedTextElement(const UICachedTextElement&) = delete;
	UICachedTextElement& operator=(const UICachedTextElement&) = delete;

	/**
	* @brief Rebuils the text texture.
	* 
	* @param renderer Pointere to the renderer used.
	*/
	void RebuildCache(SDL_Renderer* renderer) const override;

public:

	/**
	* @brief Constructor.
	* 
	* @param text Text of the element.
	* @param textfonT Font of the text.
	* @param textColor Color of the text.
	* @param textAlignment Alignment of the text.
	* @param fontFamily Font family name of the text font.
	*/
	UICachedTextElement(
		const std::string& text = "",
		TTF_Font* textFont = nullptr, 
		const SDL_Color& textColor = { 255, 255, 255, 255 },
		const UITextAlignment& textAlignment = { HorizontalAlignment::Center, VerticalAlignment::Middle },
		const std::string& fontFamily = ""
	);

	/**
	* @brief Destructor.
	*/
	virtual ~UICachedTextElement() override = default;

	/**
	* @brief Default move constructor.
	*/
	UICachedTextElement(UICachedTextElement&& other) noexcept = default;

	/**
	* @brief Sets the font of the text. Requires texture to be remade.
	* 
	* @param newFont New font for the text.
	*/
	virtual void SetTextFont(TTF_Font* newFont) override;

	/**
	* @brief Overrides the font of the text and updates the original font size.
	*
	* Use 'SetTextFont' if the font is from the same family and you want to keep the original font size.
	*
	* @param newFont New font for the text.
	* @param newFontFamily New font family name for the text.
	*/
	virtual void OverrideTextFont(TTF_Font* newFont, const std::string& newFontFamily) override;

	/**
	* @brief Sets a new text. Texture must be remade.
	*
	* @param newText New text to be displayed
	*/
	virtual void SetText(const std::string& newText) override;

	/**
	* @brief Sets the text color. Texture must be remade.
	*
	* @param color New text color.
	*/
	virtual void SetTextColor(const SDL_Color& newColor) override;

	/**
	* @brief Sets the new alignment. Texture must be remade.
	* 
	* @param newAlignment New alignment of text.
	*/
	virtual void SetTextAlignment(const UITextAlignment& newAlignment) override;

	/**
	* @brief Default move operator.
	*/
	UICachedTextElement& operator=(UICachedTextElement&& other) noexcept = default;
};

#endif // UICACHEDTEXTELEMENT_H_
