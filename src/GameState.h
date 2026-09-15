#pragma once

#ifndef GAMESTATE_H_
#define GAMESTATE_H_

#include "AppState.h"
#include "UIPanel.h"
#include "GameRenderSystem.h"
#include "AnimationSystem.h"

class KeyManager;
class Player;
class Camera;
class GameMap;

/**
* @brief Encapsulates the game state.
*/
class GameState : public AppState
{
private:
    AnimationSystem animationSystem;    /// The animation system of the game.
    mutable GameRenderSystem gameRenderer;  /// Renderer for the game state (tiles, player, enemies, etc).
    // Not fully necessary, but better than writing CLASS::GetInstance() everytime we need it.
    KeyManager& keyManager;     /// Reference to the key manager.
    Player* player = nullptr;   /// Pointer to the player.
    Camera* camera = nullptr;   /// Pointer to the camera.
    GameMap& map;               /// Reference to the game map.

    UIPanel backgroundPanel;    /// Panel for the background.

    /**
    * @brief Initialises the keyManager. Binds all controls.
    */
    void InitKeyManager();

    /**
    * @brief Initialises the player.
    */
    void InitPlayer();

    /**
    * @brief Initialises the camera.
    */
    void InitCamera();

    /**
    * @brief Initialises the map.
    */
    void InitMap();

    // No copying due to some UIElements in the UIPanel being uncopyable and references.
    GameState(const GameState&) = delete;
    GameState& operator=(const GameState&) = delete;

    // No move operator due to references.
    GameState& operator=(GameState &&) = delete;

public:

    /**
    * @brief Default constructor.
    *
    * The window and Renderer are already constructed.
    */
    GameState();

    /**
    * @brief Destructor. Calls Cleanup().
    */
    ~GameState() override;
    
    /**
    * @brief Default move constructor.
    */
    GameState(GameState&& other) = default;

    /**
    * @brief Called when the game state becomes active.
    */
    bool OnEnter();

    /**
    * @brief Called when the game state is about to get replaced.
    *
    * Does nothing.
    */
    void OnExit();

    /**
     * @brief Cleans up the state.
    */
    void Cleanup() override;

    /**
     * @brief Processes input events like keyboard or window close.
     *
     * @param e The event to be handled.
    */
    void HandleEvent(const SDL_Event& e) override;

    /**
     * @brief Updates the game state.
     *
     * @param deltaTime The delta time of the main SDL loop.
    */
    void Update(double deltaTime) override;

    /**
     * @brief Renders current game frame to the window.
     * 
     * @param renderer Pointer to the renderer used.
    */
    void Render(SDL_Renderer* renderer) const override;

    /*
    * @brief If the underlying states in the state stack should be rendered or not.
    *
    * @return 'false' for the game state.
    */
    bool RenderUnderlyingStates() const override;

    /*
    * @brief If the underlying states in the state stack should be updated or not.
    *
    * @return 'false' for the game state.
    */
    bool UpdateUnderlyingStates() const override;

    /**
    * @brief If the underlying states in the state stack should be notified of events (and handle them) or not.
    *
    * @return 'false' for the game state.
    */
    bool NotifyUnderlyingStates() const override;
};

#endif //GAMESTATE_H_