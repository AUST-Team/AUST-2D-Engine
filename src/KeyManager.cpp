#include "KeyManager.h"

#include "Command.h"
#include "MemoryTracker.h"

KeyManager::~KeyManager() { Cleanup(); }

void KeyManager::BindKey(SDL_Keycode keyCode, Command* command) { keyBindings[keyCode] = command; }

void KeyManager::OnKeyPressed(SDL_Keycode keyCode) 
{
    Command* cmd = GetCommandForKey(keyCode);

    if (!cmd)
    {
        return;
    }

    if (cmd->GetType() == CommandType::Movement)
    {
        //If the key is not yet added to the held keys, add it.
        if (std::find(heldKeys.begin(), heldKeys.end(), keyCode) == heldKeys.end()) 
        {
            heldKeys.push_back(keyCode);
        }

        Command* moveCmd = GetCommandForKey(heldKeys.front());
        if (moveCmd && moveCmd->GetType() == CommandType::Movement)
        {
            moveCmd->Execute();
        }
    }
    else 
    {
        cmd->Execute();
    }
}

void KeyManager::OnKeyReleased(SDL_Keycode keyCode) { heldKeys.erase( std::remove(heldKeys.begin(), heldKeys.end(), keyCode), heldKeys.end() ); }

Command* KeyManager::GetCommandForKey(SDL_Keycode keyCode) const 
{
    // Try to find the entry for a respective key code. Returns an iterator pointing to the entry.
    auto it = keyBindings.find(keyCode);
    // If the iterator is not at the 'end' of the collection, return the Command.
    return (it != keyBindings.end()) ? it->second : nullptr;
}

void KeyManager::Cleanup()
{
    for (auto& [_, cmd] : keyBindings)
    {
        if (cmd)
        {
            ENG_DELETE(cmd);
            cmd = nullptr;
        }
    }
    keyBindings.clear();

    isInitialisedFlag = false;
}

void KeyManager::ClearHeldKeys() { heldKeys.clear(); }

const std::vector<SDL_Keycode>& KeyManager::GetHeldKeys() const { return heldKeys; }

bool KeyManager::IsInitialised() const { return isInitialisedFlag; }

void KeyManager::SetInitialised(bool value) { isInitialisedFlag = value; }
