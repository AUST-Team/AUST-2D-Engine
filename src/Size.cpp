#include <SDL3/SDL.h>
#include <cmath>
#include <algorithm>

#include "Size.h"

Size GetScaledDimensions(SDL_Texture* texture, float maxWidth, float maxHeight, float scaleFactor, bool onlyScaleDown)
{
    if (!texture)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GetScaledSize: Texture is null.");
        return { 
            static_cast<int>(std::round(maxWidth * scaleFactor)),
            static_cast<int>(std::round(maxHeight * scaleFactor)) 
        };
    }

    float originalWidth = 0;
    float originalHeight = 0;
    if (!SDL_GetTextureSize(texture, &originalWidth, &originalHeight))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GetScaledSize: GetTextureSize failed: %s", SDL_GetError());
        return { 
            static_cast<int>(std::round(maxWidth * scaleFactor)),
            static_cast<int>(std::round(maxHeight * scaleFactor)) 
        };
    }

    const float scaledMaxW = maxWidth * scaleFactor;
    const float scaledMaxH = maxHeight * scaleFactor;

    float ratio = std::min(scaledMaxW / originalWidth, scaledMaxH / originalHeight);

    if (onlyScaleDown && ratio > 1.0f)
    {
        ratio = 1.0f;
    }

    return {
        static_cast<int>(std::round(originalWidth * ratio)),
        static_cast<int>(std::round(originalHeight * ratio))
    };
}

Size GetScaledDimensions(const SDL_FRect& srcRect, float maxWidth, float maxHeight, float scaleFactor, bool onlyScaleDown)
{
    const float baseW = srcRect.w;
    const float baseH = srcRect.h;
    const float scaleW = maxWidth / baseW;
    const float scaleH = maxHeight / baseH;

    float scale = std::min(scaleW, scaleH) * scaleFactor;

    if (onlyScaleDown && scale > 1.0f)
    {
        scale = 1.0f;
    }

    return {
        static_cast<int>(std::round(baseW * scale)),
        static_cast<int>(std::round(baseH * scale))
    };
}

bool Size::operator==(const Size& other) const { return w == other.w && h == other.h; }
bool Size::operator!=(const Size& other) const { return !(*this == other); }

Size Size::operator+(int val) const { return { w + val, h + val }; }
Size Size::operator-(int val) const { return { w - val, h - val }; }
Size Size::operator*(int val) const { return { w * val, h * val }; }
Size Size::operator/(int val) const { return { w / val, h / val }; }

Size& Size::operator+=(int val) { w += val; h += val; return *this; }
Size& Size::operator-=(int val) { w -= val; h -= val; return *this; }
Size& Size::operator*=(int val) { w *= val; h *= val; return *this; }
Size& Size::operator/=(int val) { w /= val; h /= val; return *this; }

bool FSize::operator==(const FSize& other) const { return w == other.w && h == other.h; }
bool FSize::operator!=(const FSize& other) const { return !(*this == other); }

FSize FSize::operator+(float val) const { return { w + val, h + val }; }
FSize FSize::operator-(float val) const { return { w - val, h - val }; }
FSize FSize::operator*(float val) const { return { w * val, h * val }; }
FSize FSize::operator/(float val) const { return { w / val, h / val }; }

FSize& FSize::operator+=(float val) { w += val; h += val; return *this; }
FSize& FSize::operator-=(float val) { w -= val; h -= val; return *this; }
FSize& FSize::operator*=(float val) { w *= val; h *= val; return *this; }
FSize& FSize::operator/=(float val) { w /= val; h /= val; return *this; }

bool USize::operator==(const USize& other) const { return w == other.w && h == other.h; }
bool USize::operator!=(const USize& other) const { return !(*this == other); }

USize USize::operator+(unsigned int val) const { return { w + val, h + val }; }
USize USize::operator-(unsigned int val) const { return { w - val, h - val }; }
USize USize::operator*(unsigned int val) const { return { w * val, h * val }; }
USize USize::operator/(unsigned int val) const { return { w / val, h / val }; }

USize& USize::operator+=(unsigned int val) { w += val; h += val; return *this; }
USize& USize::operator-=(unsigned int val) { w -= val; h -= val; return *this; }
USize& USize::operator*=(unsigned int val) { w *= val; h *= val; return *this; }
USize& USize::operator/=(unsigned int val) { w /= val; h /= val; return *this; }
