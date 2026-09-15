#include <SDL3/SDL.h>
#include <algorithm>

#include "GameRenderSystem.h"
#include "Assets.h"
#include "Camera.h"
#include "Player.h"
#include "TileRegistry.h"
#include "Guard.h"
#include "AnimationState.h"
#include "AnimationSystem.h"
#include "TileAnimationOverride.h"
#include "GameMap.h"
#include "ConstantConfiguration.h"

GameRenderSystem::GameRenderSystem(AnimationSystem* animationSystem) :
	animationSystem(animationSystem)
{
	if (!animationSystem)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameRenderSystem.GameRenderSystem: Passed a null animation system.");
	}
}

void GameRenderSystem::SubmitTile(TileInstance* tile, int tileX, int tileY, Camera* camera)
{
	if (!tile)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameRenderSystem.SubmitTile: Passed a null tile.");
		return;
	}

	const float scale = camera->GetScale();

	const GameConfiguration& config = Configuration::Get();
	const int tileWidth = config.tiles.width;
	const int tileHeight = config.tiles.height;

	const int tileIndex = TileRegistry::GetTileIndexByName(tile->name);
	if (tileIndex == ConstantConfiguration::invalidTileIndex)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameRenderSystem.SubmitTile: Tile has invalid index id.");
		return;
	}

	const AnimationOverrideView& override = GameMap::GetInstance().GetAnimationOverride(tileX, tileY, tileIndex);
	SDL_Texture* spriteSheet = Assets::GetSpriteSheet();

	for (const TileRenderPart& part : tile->renderParts)
	{
		Renderable r{};
		r.texture = spriteSheet;

		AnimationClip* clip = nullptr;
		AnimationState* state = nullptr;

		if (override.full)
		{
			FullAnimationOverride* o = override.full;
			if (SupportsAnimation(part, o->type))
			{
				clip = &tile->animationTable[TileReactionIndexFromType(o->type)];
				state = &o->state;
			}
		}
		else if (override.semi)
		{
			SemiAnimationOverride* o = override.semi;
			if (SupportsAnimation(part, o->type))
			{
				clip = &tile->animationTable[TileReactionIndexFromType(o->type)];
				state = animationSystem->GetOrCreate(tile, o->type);
			}
		}
		else if (tile->hasNonDefaultAnimations && !IsNone(part.defaultAnimation))
		{
			clip = &tile->animationTable[TileReactionIndexFromType(part.defaultAnimation)];
			state = animationSystem->GetOrCreate(tile, part.defaultAnimation);
		}

		r.src = clip ? ResolveTileSrcRect(part, clip, state, tileX, tileY) : part.srcRect;

		// Convert world to screen coordinates
		const float worldX = static_cast<float>((tileX + part.offset.x) * tileWidth);
		const float worldY = static_cast<float>((tileY + part.offset.y) * tileHeight);
		r.dst = SDL_FRect {
			static_cast<float>(camera->WorldToScreenX(worldX)),
			static_cast<float>(camera->WorldToScreenY(worldY)),
			std::round(part.srcRect.w * scale),
			std::round(part.srcRect.h * scale)
		};

		r.layer = part.layer;
		r.sortY = worldY;
		r.zBias = part.zBias;
		r.alpha = 255;

		layerQueues[ToIndex(r.layer)].push_back(r);
	}
}

void GameRenderSystem::SubmitPlayer(Player* player, Camera* camera)
{
	TileInstance* playerTile = TileRegistry::GetTileTypeByIndex(player->GetSprite());
	if (!playerTile)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameRenderSystem.SubmitPlayer: Invalid tile type [%d].", player->GetSprite());
		return;
	}

	TileAnimationType overrideType = player->GetAnimationOverride();
	SDL_Texture* spriteSheet = Assets::GetSpriteSheet();

	for (const TileRenderPart& part : playerTile->renderParts)
	{
		Renderable r{};
		r.texture = spriteSheet;

		TileAnimationType animType = part.defaultAnimation;
		AnimationState* animState = nullptr;
		AnimationClip* clip = nullptr;

		// Determine the animation type and state
		if (!IsNone(overrideType))
		{
			if (HasFlag(playerTile->animationMask, overrideType) && SupportsAnimation(part, overrideType))
			{
				animType = overrideType;
			}
		}

		if (!IsNone(animType))
		{
			clip = &playerTile->animationTable[TileReactionIndexFromType(animType)];

			animState = animationSystem->GetOrCreate(playerTile, animType);
		}

		// Resolve the source rectangle
		r.src = clip ? ResolveTileSrcRect(part, clip, animState, player->GetTileX(), player->GetTileY()) : part.srcRect;

		// Convert world to screen coordinates
		const float x = player->GetX();
		const float y = player->GetY();
		const float scale = camera->GetScale();

		r.dst = SDL_FRect {
			static_cast<float>(camera->WorldToScreenX(x)),
			static_cast<float>(camera->WorldToScreenY(y)),
			static_cast<float>(player->GetWidth()) * scale,
			static_cast<float>(player->GetHeight()) * scale
		};

		r.layer = RenderLayer::Actor;
		r.sortY = y;
		r.zBias = part.zBias;
		r.alpha = 255;

		layerQueues[ToIndex(r.layer)].push_back(r);
	}
}

void GameRenderSystem::SubmitGuard(Guard* guard, Camera* camera)
{
	const TileInstance* src = TileRegistry::GetTileTypeByIndex(guard->GetSprite());
	if (!src)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SubmitGuard: Invalid tile type.");
		return;
	}

	Renderable r{};
	r.texture = Assets::GetSpriteSheet();
	r.src = src->renderParts[0].srcRect;

	const float x = guard->GetX();
	const float y = guard->GetY();
	const float scale = camera->GetScale();

	r.dst = SDL_FRect {
		static_cast<float>(camera->WorldToScreenX(x)),
		static_cast<float>(camera->WorldToScreenY(y)),
		static_cast<float>(guard->GetWidth()) * scale,
		static_cast<float>(guard->GetHeight()) * scale
	};

	r.layer = RenderLayer::Actor;
	r.sortY = y;
	r.zBias = 0;

	layerQueues[ToIndex(r.layer)].push_back(r);

	// Submit alert icon as a separate renderable if active.
	if (!guard->IsAlertActive())
	{
		return;
	}

	const uint64_t now = SDL_GetTicks();
	const uint64_t elapsed = now - guard->GetAlertStartTime();

	const GameConfiguration& config = Configuration::Get();
	float guardAlertDuration = config.guard.timings.alertDuration;
	const int tileWidth = config.tiles.width;
	const int tileHeight = config.tiles.height;

	if (elapsed >= static_cast<uint64_t>(guardAlertDuration))
	{
		guard->SetAlertActive(false);
		return;
	}

	const float t = elapsed / guardAlertDuration;
	const uint8_t alpha = static_cast<uint8_t>((1.0f - t) * 255);

	const TileInstance* alertTile = TileRegistry::GetTileTypeByIndex(guard->GetAlertTileSprite());
	if (!alertTile)
	{
		return;
	}

	r.texture = Assets::GetSpriteSheet();
	r.src = alertTile->renderParts[0].srcRect;

	Size size = GetScaledDimensions(
		r.src,
		tileWidth * scale,
		tileHeight * scale,
		1.0f / 1.5f
	);

	const float screenX = static_cast <float> (camera->WorldToScreenX(x));
	const float screenY = static_cast <float> (camera->WorldToScreenY(y));

	r.dst = SDL_FRect {
		screenX + (tileWidth * scale - size.w) / 2.0f,	// center
		screenY - size.h - 10.0f * scale,				// above head
		static_cast<float>(size.w),
		static_cast<float>(size.h)
	};

	r.layer = RenderLayer::Overlay;
	r.sortY = y - 0.01f;
	r.zBias = 0;
	r.alpha = alpha;

	layerQueues[ToIndex(r.layer)].push_back(r);
}

void GameRenderSystem::SubmitRenderable(const Renderable& renderable) { layerQueues[ToIndex(renderable.layer)].push_back(renderable); }

void GameRenderSystem::Render(SDL_Renderer* renderer)
{
	cachedGlobalAnimTime = animationSystem->GetGlobalAnimationTime();

	animationSystem->BeginFrame();

	for (size_t i = 0; i < enumCount<RenderLayer>; ++i)
	{
		std::vector<Renderable>& queue = layerQueues[i];

		std::sort(queue.begin(), queue.end());

		for (const Renderable& r : queue)
		{
			if (!SDL_SetTextureAlphaMod(r.texture, r.alpha))
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameRenderSystem.Render: Failed SetTextureAlphaMod: %s", SDL_GetError());
				continue;
			}

			if (!SDL_RenderTexture(renderer, r.texture, &r.src, &r.dst))
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameRenderSystem.Render: Failed RenderTexture: %s", SDL_GetError());
			}

		}
	}

	if (!SDL_SetTextureAlphaMod(Assets::GetSpriteSheet(), 255))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameRenderSystem.Render: Failed SetTextureAlphaMod: %s", SDL_GetError());
	}
}

void GameRenderSystem::Clear()
{
	for (size_t i = 0; i < enumCount<RenderLayer>; ++i)
	{
		layerQueues[i].clear();
	}
}

void GameRenderSystem::SetAnimationSystem(AnimationSystem* newSystem)
{
	if (!newSystem)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameRenderSystem.SetAnimationSystem: Passed a null animation system.");
		return;
	}

	animationSystem = newSystem;
}

const SDL_FRect& GameRenderSystem::ResolveTileSrcRect(const TileRenderPart& part, AnimationClip* clip, AnimationState* animState, int tileX, int tileY)
{
	if (!clip || clip->frames.empty())
	{
		return part.srcRect;
	}

	float t;

	if (animState)
	{
		t = animState->elapsedTime;

	}
	else
	{
		t = cachedGlobalAnimTime;
	}

	if (clip->randomOffsetFlag)
	{
		t += animationSystem->ComputeTilePhaseOffset(tileX, tileY, clip->GetTotalDuration());
	}

	size_t frameIndex = animationSystem->GetCachedFrame(*clip, t);

	return clip->frames[frameIndex].src;
}

bool GameRenderSystem::SupportsAnimation(const TileRenderPart& part, TileAnimationType type)
{
	if (type == TileAnimationType::None)
	{
		return false;
	}
	return HasFlag(part.allowedAnimationsMask, type);
}