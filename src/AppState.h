#pragma once

#ifndef SDLSTATE_H_
#define SDLSTATE_H_

union SDL_Event;
struct SDL_Renderer;

/**
* @brief Abstract class for an App state.
* 
* Used to change between states without needing to pass around the renderer / window.
* 
* This is because I use SDL for both game stuff and GUI stuff.
*/
class AppState 
{
public:
    /**
    * @brief Default destructor.
    */
    virtual ~AppState() = default;

    /**
    * @brief Handles SDL events.
    */
    virtual void HandleEvent(const SDL_Event& e) = 0;

    /**
    * @brief Updates the current state.
    * 
    * @param deltaTime The delta time in miliseconds (ms).
    */
    virtual void Update(double deltaTime) = 0;

    /**
    * @brief Renders the current state.
    * 
    * @param renderer Pointer to the SDL Renderer used.
    */
    virtual void Render(SDL_Renderer* renderer) const = 0;

    /**
    * @brief Called when the state becomes active.
    * 
    * @return true if the state has been entered successfully, false in case of error.
    */
    virtual bool OnEnter() = 0;

    /**
    * @brief Called when the state is about to get replaced.
    * 
    */
    virtual void OnExit() = 0;

    /**
    * @brief Cleans up any resources used by the state.
    */
    virtual void Cleanup() = 0;

    /*
    * @brief If the underlying states in the state stack should be rendered or not.
    *
    * @return 'true' if the underlying states should be rendered, 'false' if otherwise.
    * Default 'false'.
    */
    virtual bool RenderUnderlyingStates() const = 0;

    /*
    * @brief If the underlying states in the state stack should be updated or not.
    *
    * @return 'true' if the underlying states should be rendered, 'false' if otherwise.
    * Default 'false'.
    */
    virtual bool UpdateUnderlyingStates() const = 0;

    /**
    * @brief If the underlying states in the state stack should be notified of events (and handle them) or not.
    * 
    * @return 'true' if the underlying states should be rendered, 'false' if otherwise.
    * Default 'false'.
    */
    virtual bool NotifyUnderlyingStates() const = 0;
};

#endif // SDLSTATE_H_
