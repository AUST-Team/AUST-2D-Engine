#include <SDL3/SDL_log.h>

#include "ConditionRegistry.h"

#include "ConditionFactory.h"
#include "Condition.h"
#include "MemoryTracker.h"

ConditionRegistry::ConditionRegistry(const std::unordered_map<ConditionKey, Condition*>& conditions) :
    conditionsByKey(conditions) {}

ConditionRegistry::ConditionRegistry(ConditionRegistry&& other) noexcept :
    conditionsByKey(std::move(other.conditionsByKey)) 
{
    other.conditionsByKey.clear();
}

ConditionRegistry::~ConditionRegistry()
{
    for (auto& [key, cond] : conditionsByKey)
    {
        if (cond)
        {
            ENG_DELETE(cond);
            cond = nullptr;
        }
    }

    conditionsByKey.clear();
}

Condition* ConditionRegistry::GetOrCreateCondition(ConditionKey key)
{
    auto it = conditionsByKey.find(key);
    if (it != conditionsByKey.end())
    {
        return it->second;
    }

    Condition* cond = ConditionFactory::CreateConditionFromKey(key);

    if(cond == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionRegistry.GetOrCreateCondition: Failed to create condition from key.");
        return nullptr;
	}

    if (cond->IsShared())
    {
        conditionsByKey.emplace(key, cond);
    }

    return cond;
}

ConditionRegistry& ConditionRegistry::operator=(ConditionRegistry&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    for (auto& [key, cond] : conditionsByKey)
    {
        if (cond)
        {
            ENG_DELETE(cond);
        }
    }

    conditionsByKey = std::move(other.conditionsByKey);
    other.conditionsByKey.clear();

    return *this;
}
