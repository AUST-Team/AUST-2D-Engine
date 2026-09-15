#pragma once

#ifndef RENDERLAYER_H_
#define RENDERLAYER_H_

#include <cstdint>
#include <string>

#include "EnumMiscs.h"

#define RENDER_LAYER_LIST(X)            \
    ENUM_SENTINEL_X_MACRO               \
    X(Ground,       1,  "ground")       \
    X(Decoration,   2,  "decoration")   \
    X(Actor,        3,  "actor")        \
    X(Foreground,   4,  "foreground")   \
    X(Overlay,      5,  "overlay")

/**
* @brief Enumeration of rendering layers.
* 
* Ideally these are in order.
*/
enum class RenderLayer : int8_t 
{
    /*
	ENUM_SENTINEL_VALUE,  /// Undefined render layer.
    Ground,     /// Ground layer.
    Decoration, /// Decoration layer.
    Actor,      /// Actor (enemies, player) layer.
    Foreground, /// Things above actors but below overlay (e.g., tree leaves, bridges).
    Overlay,    /// Overlay layer.
    */
#define X(name, val, str) name = val,
    RENDER_LAYER_LIST(X)
#undef X
    COUNT,      /// Number of elements.
};

template <>
inline constexpr bool isSentinelEnum<RenderLayer> = true;

template <>
inline constexpr bool isCountableEnum<RenderLayer> = true;

/**
* @brief Returns the render layer from a string.
* 
* @param layer String containing the name of the layer.
* 
* @return The RenderLayer of the string ("GROUND" -> Ground), or RenderLayer::ENUM_SENTINEL_VALUE if the string doesn't match anything.
*/
RenderLayer RenderLayerFromString(const std::string& layer);

/**
* @brief Returns the name of the render layer.
* 
* @param layer The render layer.
* 
* @return The string containing the name of the render layer, or std::nullopt if the layer is unknown.
*/
std::string RenderLayerToString(RenderLayer layer);

#endif // RENDERLAYER_H_