#pragma once

#ifndef UICACHEDTEXTUREELEMENT_H_
#define UICACHEDTEXTUREELEMENT_H_

#include "Size.h"

struct SDL_Texture;
struct SDL_Renderer;

/**
* @brief Class for an UI element that contains a cached texture.
*/
class UICachedTextureElement
{
protected:
	mutable SDL_Texture* cachedTexture = nullptr;		/// Pointer to the texture. OWNS IT!
	mutable FSize cachedDimensions = { 0.0f, 0.0f };	/// Cached dimensions of the texture.
	mutable bool needsUpdate = true;					/// Flag if the cache needs an update.

	// No copying allowed due to the SDL_Texture.
	UICachedTextureElement(const UICachedTextureElement&) = delete;
	UICachedTextureElement& operator=(const UICachedTextureElement&) = delete;

	/**
	 * @brief Rebuilds the cache (redraws the texture)
	 * 
	 * @param renderer Pointer to the renderer used.
	 */
	virtual void RebuildCache(SDL_Renderer* renderer) const = 0;

	/**
	 * @brief Checks if the cache needs updating, if yes, update.
	 *
	 * @param renderer Pointer to the renderer used.
	 */
	virtual void EnsureCache(SDL_Renderer* renderer) const;

public:

	/**
	* @brief Default constructor.
	*/
	UICachedTextureElement() = default;

	/**
	* @brief Destructor. Destroys the cached texture.
	*/
	virtual ~UICachedTextureElement();

	/**
	* @brief Move constructor.
	*/
	UICachedTextureElement(UICachedTextureElement&& other) noexcept;

	/**
	 * @brief Marks the cached texture as dirty and needing a rebuild.
	 */
	virtual void InvalidateCache();

	/**
	 * @brief Returns the current cached texture.
	 * 
	 * @return Pointer to the cached texture.
	 */
	virtual SDL_Texture* GetCachedTexture() const;

	/**
	 * @brief Returns the size of the cached texture.
	 * 
	 * @return A structure containing the cached width and cached height.
	 */
	virtual FSize GetCachedSize() const;

	/**
	* @brief Move operator.
	*/
	UICachedTextureElement& operator=(UICachedTextureElement&& other) noexcept;
};

#endif // UICACHEDTEXTUREELEMENT_H_
