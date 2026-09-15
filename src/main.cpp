#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "Application.h"
#include "MemoryTracker.h"
#include "ModManager.h"

/**
* @brief Main function. Initialises and runs the game.
* 
* @return 0 if game initialised, ran and closed succesfully, 1 if game cannot be initalised.
*/
int main(int argc, char* argv[])
{
#ifdef _DEBUG
	CRT_MEMORY_TRACKER_INIT();
	VLD_MEMORY_TRACKER_INIT();
	MEMORY_TRACKER_INIT();
#endif // _DEBUG

    if (!Application::Init(argc, argv))
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Main: Application Init failed: %s", SDL_GetError());
        Application::Cleanup();
        return 1;
    }

    Application::Run();
    Application::Cleanup();
    return 0;
}

/*
* Return policies:
*   - returning primitives, cheap-to-copy or local data: value (copy)
*   - returning an always valid object, but expensive to copy, can be modified by caller: reference (&) 
*   - returning possibly valid object, caller needs to check: pointer (* or std::optional), or sentinel value (for enums).
*   - returning an always valid object, where it will only be used a pointer (SDL functions): ONLY pointer (*) / constant pointer (const *)
*   - returning an always valid object, expensive to copy, but read-only: constant reference (const &)
*   - returning a dynamically created object: pointer (*)
*   - returning a polymorphic class: pointer (*)
*   - you're too cool for rules: pointer for everything (enjoy).
*/

/*
    TODO:

    Tile / Actor animations - DONE!

    Tile metadata (step sounds) or audio in general, audio events - DONE!

    Trigger conditions (more of em) - needs testing, but done in itself (unless something is broken)

    Make the UI be loaded from JSON - DONE!

    Modability (kinda like Half-Life where you can drop a folder in the game directory and then you can switch to it) - needs an actual mod to test with, but otherwise done

    Fog-of-war(like Red Alert 3?) - possibly not.

    Regions? (within the same map) - Later.

    Map / tile maker? - Not for the first release (100% gonna regret this).

    Change map interactable - Interactables are no longer used.

    Rework enemy types to be more modular, so a guard can be either a 'hold and question' (like it is currently), start battle, etc - Later.
*/