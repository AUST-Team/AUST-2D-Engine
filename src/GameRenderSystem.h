#pragma once

#ifndef GAMERENDERSYSTEM_H_
#define GAMERENDERSYSTEM_H_

#include <array>
#include <vector>

#include "Renderable.h"
#include "TileReactionType.h"

struct TileInstance;
class Camera;
class Player;
class Guard;
struct SDL_Renderer;
struct AnimationState;
class AnimationSystem;
struct TileRenderPart;
struct AnimationClip;

class GameRenderSystem
{
private:
    std::array<std::vector<Renderable>, enumCount<RenderLayer>> layerQueues;   /// Array of renderables.
    AnimationSystem* animationSystem = nullptr;   /// Pointer to the animation system. NON OWNING!
    float cachedGlobalAnimTime = 0.0f;  /// Cached global animation time.

    /**
    * @brief Returns the tile source rectangle for a tile render part of a tile type, using an animation state.
    * 
    * @param part Reference to the tile render part.
    * @param clip Pointer to the clip.
    * @param animState Pointer to the animation state.
    * @param tileX X position of the tile.
    * @param tileY Y position of the tile.
    * 
    * @return The SDL_FRect that needs to be rendered.
    */
    const SDL_FRect& ResolveTileSrcRect(const TileRenderPart& part, AnimationClip* clip, AnimationState* animState, int tileX, int tileY);

    /**
    * @brief Checks if a tile render part has a specific animation.
    * 
    * @param part Reference to the tile render part.
    * @param type The animation type to check for.
    * 
    * @return 'true' if the part supports said type, 'false' if otherwise.
    */
    bool SupportsAnimation(const TileRenderPart& part, TileAnimationType type);

public:

    /**
    * @brief Constructor.
    * 
    * @param animationSystem Pointer to the animation system.
    */
    GameRenderSystem(AnimationSystem* animationSystem);

    /**
    * @brief Submits a tile to be rendered.
    * 
    * @param tile Pointer to the tile. NON OWNING!
    * @param tileX X position of the tile.
    * @param tileY Y position of the tile.
    * @param camera Pointer to the camera. NON OWNING!
    */
    void SubmitTile(TileInstance* tile, int tileX, int tileY, Camera* camera);

    /**
    * @brief Submits the player to be rendered.
    * 
    * @param player Pointer to the player. NON OWNING!
    * @param camera Pointer to the camera. NON OWNING!
    */
    void SubmitPlayer(Player* player, Camera* camera);

    /**
    * @brief Submits a guard to be rendered.
    * 
    * @param guard Pointer to the guard. NON OWNING!
    * @param camera Pointer to the camera. NON OWNING!
    */
    void SubmitGuard(Guard* guard, Camera* camera);

    /**
    * @brief Submits a renderable to be rendered.
    * 
    * @param renderable Reference to the renderable.
    */
    void SubmitRenderable(const Renderable& renderable);

    /**
    * @brief Renders the 'queue', sorted by layer and depth.
    * 
    * @param renderer Pointer to the renderer used.
    */
    void Render(SDL_Renderer* renderer);

    /**
    * @brief Clears the 'queue' for the next frame.
    */
    void Clear();

    /**
    * @brief Sets the animation system of the renderer.
    * 
    * @param newSystem Pointer to the new system. NON OWNING!
    */
    void SetAnimationSystem(AnimationSystem* newSystem);
};

#endif // GAMERENDERSYSTEM_H_