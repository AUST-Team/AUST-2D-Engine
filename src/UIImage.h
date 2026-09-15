#pragma once

#ifndef UIIMAGE_H_
#define UIIMAGE_H_

#include "UICachedTextureElement.h"
#include "UIElement.h"

struct SDL_Surface;

/**
* @brief Class that holds a cached texture element.
* 
* This class holds a surface that is given, for an image that references a tile, use UITileImage.
*/
class UIImage : public UIElement, public UICachedTextureElement
{
private:
    mutable SDL_Surface* originalSurface = nullptr; /// The SDL_Surface of the image.
    uint8_t transparency = 0;  /// Transparency of the image.

    // No copying allowed due to the SDL_Texture / Surface.
    UIImage(const UIImage&) = delete;
    UIImage& operator=(const UIImage&) = delete;

    /**
    * @brief Rebuilds the cache of the image.
    * 
    * @param renderer Pointer to the SDL_Renderer used.
    */
    void RebuildCache(SDL_Renderer* renderer) const override;

protected:

    /**
    * @brief Renders the image.
    *
    * @param renderer Pointer to the SDL_Renderer used.
    */
    void OnRender(SDL_Renderer* renderer) const override;

public:

    /**
    * @brief Constructor.
    * 
    * @param id ID of the element.
    * @param bounds Bounds of the image element.
    * @param backgroundColor Background color of the image element.
    * @param borderColor Border color of the iamge element.
    * @param surface Pointer to an SDL_Surface containing the surface.
    * @param transparency Transparency of the image.
    */
    explicit UIImage(
        const std::string id = "",
        const SDL_FRect& bounds = { 0.0f, 0.0f, 0.0f, 0.0f },
        const SDL_Color& backgroundColor = { 0, 0, 0, 0 },
        const SDL_Color& borderColor = { 255, 255, 255, 255 },
        SDL_Surface* surface = nullptr,
        uint8_t transparency = 255
    );

    /**
    * @brief Move constructor.
    */
    UIImage(UIImage&& other) noexcept;

    /**
    * @brief Destructor.
    */
    ~UIImage() override;

    /**
    * @brief Sets the surface of the image.
    * 
    * @param newSurface The new surface of the element. THIS IS NOT THE TEXTURE!
    */
    void SetImageSurface(SDL_Surface* newSurface);

    /**
    * @brief Returns the surface of the image.
    * 
    * @return The surface of the image or nullptr if it wasn't yet set. THIS IS NOT THE TEXTURE!
    */
    SDL_Surface* GetImageSurface() const;

    /**
    * @brief Sets the bounds of the image.
    *
    * Invalidates the cache.
    *
    * @param newBounds New bounds of the image.
    */
    void SetBounds(const SDL_FRect& newBounds) override;

    /**
    * @brief Sets the transparency of the image.
    *
    * Invalidates the cache.
    *
    * @param newTransparency New transparency of the image.
    */
    void SetTransparency(uint8_t newTransparency);

    /**
    * @brief Returns the transparency of the image.
    * 
    * @return The transparency of the image.
    */
    uint8_t GetTransparency() const;

    /**
    * @brief Move operator.
    */
    UIImage& operator=(UIImage&& other) noexcept;
};

#endif // UIIMAGE_H_