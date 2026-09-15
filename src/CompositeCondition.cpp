#include <SDL3/SDL_log.h>

#include "CompositeCondition.h"
#include "MemoryTracker.h"

CompositeCondition::CompositeCondition(CompositeType t, const std::vector<Condition*>& conditions) :
	conditions(conditions),
    type(t) {}

CompositeCondition::CompositeCondition(CompositeType t, std::vector<Condition*>&& conditions) :
    conditions(std::move(conditions)),
    type(t) {}

CompositeCondition::CompositeCondition(CompositeCondition&& other) noexcept :
    conditions(std::move(other.conditions)),
    type(other.type) { other.conditions.clear(); }

CompositeCondition::~CompositeCondition()
{
    for (Condition* c : conditions)
    {
        if (c && !c->IsShared())
        {
            ENG_DELETE(c);
        }
    }
    conditions.clear();
}

void CompositeCondition::AddCondition(Condition* cond) 
{
    if (!cond)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "CompositeCondition.AddCondition: Attempted to add a null condition.");
        return;
    }
    conditions.push_back(cond); 
}

std::vector<Condition*> CompositeCondition::GetConditions() const { return conditions; }

bool CompositeCondition::IsSatisfied(Trigger* trigger, Player* player, MapUpdateFlags updateMask) const
{
    switch (type)
    {
        case CompositeType::AND:
        {
            for (const Condition* c : conditions)
            {
                if (!c->IsSatisfied(trigger, player, updateMask))
                {
                    return false;
                }
            }
            return true;
        }

        case CompositeType::OR:
        {
            for (const Condition* c : conditions)
            {
                if (c->IsSatisfied(trigger, player, updateMask))
                {
                    return true;
                }
            }
            return false;
        }

        case CompositeType::NOT:
        {
            if (conditions.size() != 1)
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "CompositeCondition.IsSatisfied: NOT condition must have exactly one child.");
                return false;
            }

            return !conditions[0]->IsSatisfied(trigger, player, updateMask);
        }

        default:
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "CompositeCondition.IsSatisfied: Unknown composite type.");
            return false;
        }
    }
}

MapUpdateFlags CompositeCondition::GetUpdateFlags() const
{
    MapUpdateFlags flags = MapUpdateFlags::None;
    for (const Condition* c : conditions)
    {
        flags |= c->GetUpdateFlags();
    }
    return flags;
}

bool CompositeCondition::IsShared() const { return false; }

uint64_t CompositeCondition::GetSpatialRadius() const
{
    uint64_t maxR = 0;
    for (const Condition* c : conditions)
    {
        maxR = std::max(maxR, c->GetSpatialRadius());
    }
    return maxR;
}

CompositeCondition& CompositeCondition::operator=(CompositeCondition&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    conditions = std::move(other.conditions);
    type = other.type;

    other.conditions.clear();

    return *this;
}
