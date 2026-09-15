#include "GuardManager.h"

#include "GuardState.h"
#include "Guard.h"
#include "GuardChaseState.h"
#include "GuardSearchState.h"
#include "GuardCooldownState.h"
#include "MemoryTracker.h"

#include <SDL3/SDL.h>

GuardManager::~GuardManager() { ClearGuards(); }

void GuardManager::Update(double deltaTime)
{
    for (Guard* guard : guards)
    {
        guard->Update(deltaTime);
    }
}

void GuardManager::RegisterGuard(Guard* guard) { guards.push_back(guard); }

void GuardManager::UnregisterGuard(Guard* guard) 
{ 
    auto it = std::find(guards.begin(), guards.end(), guard);
    if (it != guards.end())
    {
        ENG_DELETE(*it);
        guards.erase(it);
    }
}

Guard* GuardManager::GetGuard(size_t index) const
{
    if (index >= guards.size())
    {
        return nullptr;
    }

    return guards[index];
}

std::vector<Guard*>& GuardManager::GetGuards() { return guards; }

const std::vector<Guard*>& GuardManager::GetGuards() const { return guards; }

void GuardManager::ClearGuards()
{
    for (Guard*& guard : guards)
    {
        if (guard)
        {
            ENG_DELETE(guard);
            guard = nullptr;
        }
    }
    guards.clear();
}

size_t GuardManager::GetGuardCount() const { return guards.size(); }

size_t GuardManager::GetAlertedGuardCount() const
{
    size_t count = 0;
    for (Guard* guard : guards)
    {
        if (IsGuardInAlertedState(*guard))
        {
            ++count;
        }
    }
    return count;
}

bool GuardManager::IsGuardInAlertedState(const Guard& guard) const { return guard.GetCurrentState()->GetStateType() == GuardStateType::Alert; }