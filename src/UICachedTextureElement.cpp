#include <SDL3/SDL.h>

#include "UICachedTextureElement.h"

#include "Size.h"
#include "MemoryTracker.h"

UICachedTextureElement::~UICachedTextureElement()
{
	if (cachedTexture)
	{
		ENG_SDL_DestroyTexture(cachedTexture);
		cachedTexture = nullptr;
	}
}

UICachedTextureElement::UICachedTextureElement(UICachedTextureElement&& other) noexcept :
	cachedTexture(other.cachedTexture),
	cachedDimensions(other.cachedDimensions),
	needsUpdate(other.needsUpdate) { other.cachedTexture = nullptr; }

void UICachedTextureElement::InvalidateCache() { needsUpdate = true; }

SDL_Texture* UICachedTextureElement::GetCachedTexture() const { return cachedTexture; }

FSize UICachedTextureElement::GetCachedSize() const { return cachedDimensions; }

UICachedTextureElement& UICachedTextureElement::operator=(UICachedTextureElement&& other) noexcept
{
	if (this == &other)
	{
		return *this;
	}

	ENG_SDL_DestroyTexture(cachedTexture);

	cachedTexture = other.cachedTexture;
	cachedDimensions = other.cachedDimensions;
	needsUpdate = other.needsUpdate;

	other.cachedTexture = nullptr;

	return *this;
}

void UICachedTextureElement::EnsureCache(SDL_Renderer* renderer) const
{
	if (needsUpdate)
	{
		if (cachedTexture)
		{
			ENG_SDL_DestroyTexture(cachedTexture);
			cachedTexture = nullptr;
		}

		RebuildCache(renderer);

		needsUpdate = false;
	}
}
