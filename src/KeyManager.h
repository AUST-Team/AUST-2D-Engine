#pragma once

#ifndef KEYMANAGER_H_
#define KEYMANAGER_H_

#include <SDL3/SDL_keycode.h>
#include <unordered_map>
#include <vector>

class Command;

/**
* @brief Encapsulates key binding, as well as the actions on key press and key release.
*/
class KeyManager
{
private:
    std::unordered_map<SDL_Keycode, Command*> keyBindings = {};  /// Map of all keybinds.
    std::vector<SDL_Keycode> heldKeys = {};    /// Vector of all held keys.

    bool isInitialisedFlag = false;     /// Flag if the keybindings are all already initialised.

    /**
    * @brief Default constructor.
    */
    KeyManager() = default;

    /**
    * @brief Destructor. Calls Cleanup();
    */
    ~KeyManager();

    // No copying or moving allowed due to singleton pattern.
    KeyManager(const KeyManager&) = delete;
    KeyManager& operator=(const KeyManager&) = delete;
    KeyManager(KeyManager&&) = delete;
    KeyManager& operator=(KeyManager&&) = delete;

public:

    /**
    * @brief Returns a reference to the key manager instance.
    * 
    * @return A reference to the KeyManager instance.
    */
    static KeyManager& GetInstance() noexcept
    {
        static KeyManager instance;
        return instance;
    }

    /**
    * @brief Binds a key to a certain Command.
    *
    * @param keyCode Key code of the respective key.
    * @param command The command to be executed on key press.
    */
    void BindKey(SDL_Keycode keyCode, Command* command);

    /**
    * @brief Activates on key press.
    * 
    * Call from the method that handles events.
    *
    * @param keyCode Key code of the pressed key.
    */
    void OnKeyPressed(SDL_Keycode keyCode);

    /**
    * @brief Activates on key release.
    * 
    * Call from the method that handles events.
    *
    * @param keyCode Key code of the released key.
    */
    void OnKeyReleased(SDL_Keycode keyCode);

    /**
    * @brief Returns the Command for a particular key code.
    *
    * @param keycode Key code of the key we want the command for.
    *
    * @return A pointer to the Command of the respective key.
    */
    Command* GetCommandForKey(SDL_Keycode keyCode) const;

    /**
    * @brief Cleans up the key manager.
    */
    void Cleanup();

    /**
    * @brief Clears all held keys.
    */
    void ClearHeldKeys();

    /**
    * @brief Returns the list of currently held keys.
    *
    * @return std::list of the currently held keys.
    */
    const std::vector<SDL_Keycode>& GetHeldKeys() const;

    /**
    * @brief Returns if the keybindings have been initialised or not.
    * 
    * @return 'true' if the keybindings have been initialised, 'false' if otherwise.
    */
    bool IsInitialised() const;

    /**
    * @brief Sets the 'isInitialised' flag to the provided value;
    * 
    * @param value The value for the flag.
    */
    void SetInitialised(bool value);
};

#endif // KEYMANAGER_H_