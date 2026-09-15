#pragma once

#ifndef UILABELMASKED_H_
#define UILABELMASKED_H_

#include "UILabel.h"

/**
* @brief An UILabel with the ability to have a image over the text.
*/
class UILabelMasked : public UILabel
{
private:
	SDL_Texture* backgroundTexture = nullptr;	/// Texture to mask over the text.

	// No copying allowed due to cached texture and background texture.
	UILabelMasked(const UILabelMasked&) = delete;
	UILabelMasked& operator=(const UILabelMasked&) = delete;

protected:

	/**
	* @brief Updates the cached mask texture.
	* 
	* @param renderer Pointer to the renderer used.
	*/
	void RebuildCache(SDL_Renderer* renderer) const override;

public:

	/**
	* @brief Constructor.
	*
	* @param id ID of the element.
	* @param bounds The bounds (coordinates and size) of the label.
	* @param backgroundColor The color of the background.
	* @param borderColor The color of the border.
	* @param text The text of the label.
	* @param textFont The font of the text.
	* @param textColor The color of the text.
	* @param textAlignment Alignment of the text.
	* @param fontFamily Family of the font.
	* @param backgroundTexture Texture of the image to be masked.
	*/
	explicit UILabelMasked(
		const std::string id = "",
		const SDL_FRect& bounds = { 0.0f, 0.0f, 0.0f, 0.0f },
		const SDL_Color& backgroundColor = { 0, 0, 0, 0 },
		const SDL_Color& borderColor = { 255, 255, 255, 255 },
		const std::string& text = "",
		TTF_Font* textFont = nullptr,
		const SDL_Color& textColor = { 255, 255, 255, 255 },
		const UITextAlignment& textAlignment = { HorizontalAlignment::Center, VerticalAlignment::Middle },
		const std::string& fontFamily = "",
		SDL_Texture* backgroundTexture = nullptr
	);

	/**
	* @brief Destructor.
	*/
	~UILabelMasked() override;

	/**
	* @brief Move constructor.
	*/
	UILabelMasked(UILabelMasked&& other) noexcept;

	/**
	* @brief Sets the background texture.
	* 
	* @param newTexture Texture of the background.
	*/
	void SetBackgroundTexture(SDL_Texture* newTexture);

	/**
	* @brief Move operator.
	*/
	UILabelMasked& operator=(UILabelMasked&& other) noexcept;
};


#endif // UILABELMASKED_H_