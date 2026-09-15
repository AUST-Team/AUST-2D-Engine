#pragma once

#ifndef UILABELPOOL_H_
#define UILABELPOOL_H_

#include <vector>

#include "UILabel.h"

/**
* @brief Class for a pool of labels.
*/
class UILabelPool
{
private:
    mutable std::vector<UILabel> labels = {};

    // No copying allowed due to the labels (cached texture).
    UILabelPool(const UILabelPool&) = delete;
    UILabelPool& operator=(const UILabelPool&) = delete;

public:

    /**
    * @brief Default constructor.
    */
    UILabelPool() = default;

    /**
    * @brief Default move constructor.
    */
    UILabelPool(UILabelPool&& other) noexcept = default;

    /**
    * @brief Default destructor.
    */
    ~UILabelPool() = default;

    /**
    * @brief Renders all labels.
    * 
    * @param renderer Pointer to the renderer used.
    * @param yOffset Offset on the Y coordinate for rendering.
    */
    void Render(SDL_Renderer* renderer, float yOffset = 0.0f) const;

    /**
    * @brief Resizes the label pool.
    * 
    * @param newSize New size of the label pool.
    */
    void Resize(size_t newSize);

    /**
    * @brief Clears the label pool.
    */
    void Clear();

    /**
    * @brief Checks if the label pool is empty.
    * 
    * @return 'true' if the pool is empty, 'false' if otherwise.
    */
    bool Empty() const;

    /**
    * @brief Invalidates the caches of the labels.
    */
    void InvalidateCache();

    /**
    * @brief Sets the font of the labels.
    * 
    * @param newFont The new font of the labels.
    */
    void SetTextFont(TTF_Font* newFont);

    /**
    * @brief Sets the text color of the labels.
    * 
    * @param newColor The new text color of the labels.
    */
    void SetTextColor(const SDL_Color& newColor);

    /**
    * @brief Sets the border color of the labels.
    * 
    * @param newColor The new border of the labels.
    */
    void SetBorderColor(const SDL_Color& newColor);

    /**
    * @brief Sets the alignment of the labels.
    * 
    * @param newAlignment The new alignment of the labels.
    */
    void SetTextAlignment(UITextAlignment newAlignment);

    /**
    * @brief Returns the size of the label pool.
    * 
    * @return The size of the label pool.
    */
    size_t Size() const;

    /**
    * @brief Returns a reference to the label pool.
    * 
    * @return A reference to the vector of labels.
    */
    std::vector<UILabel>& Get();

    /**
    * @brief Returns a read-only reference to the label pool.
    * 
    * @return A const reference to the vector of labels.
    */
    const std::vector<UILabel>& Get() const;

    /**
    * @brief Indexing operator.
    */
    UILabel& operator[](size_t index);

    const UILabel& operator[](size_t index) const;

    /**
    * @brief Default move operator.
    */
    UILabelPool& operator=(UILabelPool&& other) noexcept = default;
};

#endif // UILABELPOOL_H_
