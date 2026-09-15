#pragma once

#ifndef APPRENDERSYSTEM_H_
#define APPRENDERSYSTEM_H_

#include <string>

#include "Configuration.h"
#include "Size.h"

struct SDL_Window;
struct SDL_Renderer;

/**
* @brief Holds a global reference to the window and renderer.
* 
* Not to be confused with the GameRenderSystem, which is only used for the game state.
*/
class AppRenderSystem 
{
private:
    SDL_Window* window = nullptr;       /// Pointer to the window.
    SDL_Renderer* renderer = nullptr;   /// Pointer to the renderer.

    // Note: These do not update automatically, call UpdateWindowSize().
    Size dimensions = { Configuration::Get().window.width, Configuration::Get().window.height };    /// Size of the window in pixels.

    Size pendingDimensions = { Configuration::Get().window.width, Configuration::Get().window.height }; /// Pending dimensions for change.

    bool pendingSizeChangeFlag = false; /// Flag if the window has any pending size changes.
    bool pendingFullscreenFlag = false; /// Flag if the window should be made fullscreen.
    bool fullscreenStatusFlag = false;  /// Flag if the window should be made fullscreen from normal (true) or from fullscreen to normal (false).

    /**
    * @brief Default constructor.
    */
    AppRenderSystem() = default;

    /**
    * @brief Destructor. Calls Cleanup();
    */
    ~AppRenderSystem();

    // No copying or moving allowed due to the singleton pattern.
    AppRenderSystem(const AppRenderSystem&) = delete;
    AppRenderSystem& operator=(const AppRenderSystem&) = delete;
    AppRenderSystem(AppRenderSystem&&) = delete;
    AppRenderSystem& operator=(AppRenderSystem&&) = delete;

public:

    /**
    * @brief Returns the instance of the class.
    * 
    * @return A reference to the instance of the class.
    */
    static AppRenderSystem& GetInstance() noexcept
    {
        static AppRenderSystem instance;
        return instance;
    }

    /**
    * @brief Initialises the window and renderer.
    * 
    * @param title Title of the window.
    * @param width Width of the window.
    * @param height Height of the window.
    * 
    * @return true if the initialisation was successful, false otherwise.
    */
    bool Init(
        const std::string& title = Configuration::Get().window.name,
        int width = Configuration::Get().window.width,
        int height = Configuration::Get().window.height
    );

    /**
    * @brief Cleans up the window and renderer.
    */
    void Cleanup();

    /**
    * @brief Updates the window size automatically.
    */
    void UpdateWindowSize();

    /**
    * @brief Processes any pending window changes.
    * 
    * \/!\ DOES NOT CHANGE THE CACHED WINDOW SIZE \/!\
    */
    void ProcessPendingWindowChanges();

    /**
    * @brief Returns the pointer to the renderer.
    * 
    * @return The pointer to the renderer.
    */
    SDL_Renderer* GetRenderer() const;

    /**
    * @brief Retuns the pointer to the window.
    * 
    * @return The pointer to the renderer.
    */
    SDL_Window* GetWindow() const;

    /**
    * @brief Returns the width of the window.
    * 
    * @return The width of the window in pixels.
    */
    int GetWidth() const;

    /**
    * @brief Returns the height of the window.
    *
    * @return The width of the height in pixels.
    */
    int GetHeight() const;

    /**
    * @brief Returns the usable bounds of the window.
    * 
    * The usable bounds is the unobstructed bounds of the window (size of the window without the taskbar and other such elements)
    * 
    * These bounds are not cached.
    * 
    * @return The SDL_Rect representing the usable bounds of the window.
    */
    Size GetSafeWindowSize() const;

    /**
    * @brief Schedules a window size change.
    * 
    * @param newDimensions Dimensions to change to.
    */
    void ScheduleWindowSizeChange(Size newDimensions);

    /**
    * @brief Schedules the window to change the fullscreen.
    * 
    * @param fullscreen If the window should be fullscreened ('true') or minimized ('false').
    */
    void ScheduleFullScreen(bool fullscreen);

    /**
    * @brief Checks if the window is fullscreen.
    * 
    * @return 'true' if the screen if fullscreen, 'false' if otherwise.
    */
    bool IsFullscreen() const;

    /**
    * @brief Shows or hides the window.
    * 
    * @param show 'TRUE' to show the window, 'FALSE' to hide the window.
    */
    void ShowWindow(bool show);
};

#endif // APPRENDERSYSTEM_H_