#pragma once

#ifndef SDLSTATEMANAGER_H_
#define SDLSTATEMANAGER_H_

#include <vector>

#include "StackAction.h"

union SDL_Event;
struct SDL_Renderer;

/**
* Encapsulates the state transition logic as well as the main app loop.
*/
class AppStateManager 
{
private:
    std::vector<StackAction> pendingStackActions;   /// Queue of stack actions.
    std::vector<AppState*> stateStack ;  /// Vector (stack) of pointers to current states. OWNS THEM!      
    bool running = false;       /// Flag if the main app loop is running.
    bool pendingQuit = false;   /// Flag if there is a pending quit.

    /**
    * @brief Default constructor.
    */
    AppStateManager() = default;

    /**
    * @brief Default destructor.
    */
    ~AppStateManager() = default;

    //No copying or moving allowed due to singleton pattern.
    AppStateManager(const AppStateManager&) = delete;
    AppStateManager& operator=(const AppStateManager&) = delete;
    AppStateManager(AppStateManager&&) = delete;
    AppStateManager& operator=(AppStateManager&&) = delete;

    /**
    * @brief Pushes a state onto the stack.
    *
    * @param state Unique pointer of the new state.
    */
    void PushState(AppState* state);

    /**
    * @brief Pops a state from the stack.
    */
    void PopState();

public:

    /**
    * @brief Returns the instance of the class.
    *
    * @return A reference to the instance.
    */
    static AppStateManager& GetInstance() noexcept
    {
        static AppStateManager instance;
        return instance;
    }

    /**
    * @brief Runs the main SDL loop.
    */
    void Run();

    /**
    * @brief Quits the loop (and the program).
    */
    void Quit();

    /**
    * @brief Handles the SDL event in case of QUIT, forwards any other event to the current state.
    */
    void HandleEvents();

    /**
    * @brief Cleans up resources.
    */
    void Cleanup();

    /**
    * @brief Forces changing the current state to the new state.
    *
    * Don't use unless you really know what you're doing, or the main app loop hasn't started yet.
    * 
    * \/\!\ Empties the state stack /!\
    *
    * @param newState The new state to change to.
    */
    void ChangeState(AppState* newState);

    /**
    * @brief Handles all pending actions.
    */
    void HandlePendingActions();

    /**
    * @brief Updates all states in the state stack.
    * 
    * A state is updated if the previous state in the stack has the UpdateUnderflyingStates method return true.
    * 
    * @param deltaTime The deltaTime of the main SDL loop.
    */
    void UpdateStates(double deltaTime);

    /**
    * @brief Renders all states in the state stack.
    * 
    * A state is updated if the previous state in the stack has the RenderUnderflyingStates method return true.
    * 
    * @param renderer Pointer to the SDL Renderer used.
    */
    void RenderStates(SDL_Renderer* renderer) const;

    /**
    * @brief Notifies all states in the stack state of an event.
    * 
    * A state is updated if the previous state in the stack has the NotifyUnderlyingStates method return true.
    * 
    * @param e Event to be handled.
    */
    void HandleEventStates(const SDL_Event& e) const;

    /**
    * @brief Schedules a state change (after all the current state related things have been handled).
    *
    * \/\!\ Empties the state stack /!\
    * 
    * \/\!\ Nullifies any scheduled pops /!\
    * 
    * @param newState The new state to change to.
    */
    void ScheduleStateChange(AppState* newState);

    /**
    * @brief Schedules a quit.
    * 
    * A scheduled quit happens after the current state's methods finish (handle events, update, render).
    */
    void ScheduleQuit();

    /**
    * @brief Schedules a pop.
    * 
    * A scheduled pop happens after the current state's methods finish.
    */
    void SchedulePop();

    /**
    * @brief Schedules a push.
    * 
    * @param state The state to push onto the stack. Will become the active state.
    */
    void SchedulePushState(AppState* state);

    /**
    * @brief Checks if the game is paused <=> the top of the state stack is PauseMenuState
    * 
    * @return 'true' if the current state is PauseMenuState, false if stack is empty or there is a different state.
    */
    bool IsCurrentStatePause() const;
};

#endif // SLDSTATEMANAGER_H_