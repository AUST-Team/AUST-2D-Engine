#pragma once

#ifndef GUARDMANAGER_H_
#define GUARDMANAGER_H_

#include <vector>

class Guard;
struct SDL_Renderer;

/**
* @brief Keeps a centralised reference to all guards.
*/
class GuardManager 
{
private:
    std::vector<Guard*> guards = {}; /// Vector for pointers to guards. OWNING!

    /**
    * @brief Default constructor.
    */
    GuardManager() = default;

    /**
    * @brief Destructor. Destroy all guards.
    */
    ~GuardManager();

    // No copying or moving allowed due to singleton pattern.
    GuardManager(const GuardManager&) = delete;
    GuardManager& operator=(const GuardManager&) = delete;
    GuardManager(GuardManager&&) = delete;
    GuardManager& operator=(GuardManager&&) = delete;

    /**
    * @brief Checks if the guard is in an alerted state.
    * 
    * @param guard Constant reference to the guard.
    */
    bool IsGuardInAlertedState(const Guard& guard) const;

public:

    /**
    * @brief Returns the instance of the guard manager.
    * 
    * @return Reference to the guard manager instance.
    */
    static GuardManager& GetInstance() noexcept
    {
        static GuardManager instance;
        return instance;
    }

    /**
    * @brief Updates the guards.
    * 
    * @param deltaTime The delta time of the main app loop.
    */
    void Update(double deltaTime);

    /**
    * @brief Registers a guard in the manager.
    * 
    * @param guard Pointer to the guard.
    */
    void RegisterGuard(Guard* guard);

    /**
    * @brief Unregisteres a guard from the manager.
    * 
    * @param guard Pointer to the guard.
    */
    void UnregisterGuard(Guard* guard);

    /**
    * @brief Returns the guard with the respective index.
    *
    * @param index Index of the guard.
    *
    * @return Pointer to the guard or nullptr if the index is outside the vector bounds.
    */
    Guard* GetGuard(size_t index) const;

    /**
    * @brief Returns all the guards.
    * 
    * @return Reference to a vector containing the guard pointers.
    */
    std::vector<Guard*>& GetGuards();

    /**
    * @brief Returns all the guards.
    *
    * @return A constant reference to a vector containing the guard pointers.
    */
    const std::vector<Guard*>& GetGuards() const;

    /**
    * @brief Clears all the guards.
    */
    void ClearGuards();

    /**
    * @brief Returns the number of guards.
    * 
    * @return The number of guards.
    */
    size_t GetGuardCount() const;

    /**
    * @brief Returns the number of guards in an alerted state.
    * 
    * @return The number of guards in an alerted state.
    */
    size_t GetAlertedGuardCount() const;
};

#endif // GUARDMANAGER_H_
