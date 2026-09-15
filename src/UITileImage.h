#pragma once

#ifndef UITILEIMAGE_H_
#define UITILEIMAGE_H_

#include "UIElement.h"
#include "UICachedTextureElement.h"
#include "ConstantConfiguration.h"

/**
* @brief Class that holds a tile index for drawing on screen.
*
* This class holds a tile index. For an image that uses a given surface, use UIImage.
*/
class UITileImage : public UIElement, public UICachedTextureElement
{
private:
    int tileIndex = ConstantConfiguration::invalidTileIndex;  /// Index of the tile.
    uint8_t transparency = 255;

    // No copying allowed due to the SDL_Texture / Surface.
    UITileImage(const UITileImage&) = delete;
    UITileImage& operator=(const UITileImage&) = delete;

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
    * @param overlayColor Overlay color of the image.
    */
    explicit UITileImage(
        const std::string id = "",
        const SDL_FRect& bounds = { 0.0f, 0.0f, 0.0f, 0.0f },
        const SDL_Color& backgroundColor = { 0, 0, 0, 0 },
        const SDL_Color& borderColor = { 255, 255, 255, 255 },
        int tileIndex = ConstantConfiguration::invalidTileIndex,
        uint8_t transparency = 255
    );

    /**
    * @brief Default move constructor.
    */
    UITileImage(UITileImage&& other) noexcept = default;

    /**
    * @brief Default destructor.
    */
    ~UITileImage() override = default;

    /**
    * @brief Handles an event.
    *
    * @param e The SDL_Event to be handled.
    */
    void HandleEvent(const SDL_Event& e) override;

    /**
    * @brief Updates the UI elemement (animations, etc).
    *
    * @param deltaTile The delta time of the main SDL loop (ms).
    */
    void Update(double deltaTime) override;

    /**
    * @brief Resizes the element.
    *
    * @param widthScale The scale of the width (newWidth / oldWidth).
    * @param heightScale The scale of the height (newHeight / oldHeight).
    */
    void Resize(float widthScale, float heightScale) override;

    /**
    * @brief Sets the tile index of the image.
    *
    * @param newIndex New index of the tile.
    */
    void SetTileIndex(int newIndex);

    /**
    * @brief Returns the tile index of the image.
    * 
    * @return The tile index of the time.
    */
    int GetTileIndex() const;

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
    * @brief Default move operator.
    */
    UITileImage& operator=(UITileImage&& other) noexcept = default;
};

#endif // UITILEIMAGE_H_