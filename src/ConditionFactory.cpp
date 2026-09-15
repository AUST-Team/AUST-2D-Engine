#include <SDL3/SDL_log.h>

#include "ConditionFactory.h"

#include "CompositeCondition.h"
#include "Miscs.h"
#include "InteractCondition.h"
#include "ProximityCondition.h"
#include "SteppedOnCondition.h"
#include "FacingCondition.h"
#include "ConditionRegistry.h"
#include "MemoryTracker.h"
#include "TimedCondition.h"
#include "CooldownCondition.h"
#include "MaxActivationsCondition.h"

using json = nlohmann::json;

Condition* ConditionFactory::CreateConditionFromJSON(const json& conditionsJSON, ConditionRegistry& conditionRegistry)
{
    /*
        What this functions expects (examples):

        "conditionsJSON": {
            "type": "interact"
        }

        "conditionsJSON": {
            "and": [
              {"type": "interact"},
              {"type": "facing", "facing": "lookAt"}
            ]
        }

        "conditionsJSON": {
            "or": [
              {"type": "interact"},
              {"type": "steppedOn"},
              {"and": [
                {"type": "proximity", "range": 4},
                {"type": "facing", "facing": "away"}
              ]}
            ]
          }
    */

    if (!conditionsJSON.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionFactory.CreateConditionFromJSON: Expected object, got non-object.");
        return nullptr;
    }

    if (conditionsJSON.contains("and"))
    {
        if (!conditionsJSON["and"].is_array())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionFactory.CreateConditionFromJSON: Composite 'AND' is not array.");
            return nullptr;
        }

        std::vector<Condition *> children;
        for (const json& c : conditionsJSON["and"])
        {
            if (!c.is_object())
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionFactory.CreateConditionFromJSON: Child in AND composition is not object.");
                return nullptr;
            }

            Condition* child = CreateConditionFromJSON(c, conditionRegistry);
            if (!child)
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionFactory.CreateConditionFromJSON: Failed child creation for AND composition.");
                return nullptr;
            }
            children.push_back(child);
        }
        return ENG_NEW(CompositeCondition, CompositeType::AND, std::move(children));
    }

    if (conditionsJSON.contains("or"))
    {
        if (!conditionsJSON["or"].is_array())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionFactory.CreateConditionFromJSON: Composite 'OR' is not array.");
            return nullptr;
        }

        std::vector<Condition*> children;
        for (const json& c : conditionsJSON["or"])
        {
            if (!c.is_object())
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionFactory.CreateConditionFromJSON: Child in OR composition is not object.");
                return nullptr;
            }

            Condition* child = CreateConditionFromJSON(c, conditionRegistry);
            if (!child)
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionFactory.CreateConditionFromJSON: Failed child creation for OR composition.");
                return nullptr;
            }
            children.push_back(child);
        }
        return ENG_NEW(CompositeCondition, CompositeType::OR, std::move(children));
    }

    if (conditionsJSON.contains("not"))
    {
        if (!conditionsJSON["not"].is_object())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionFactory.CreateConditionFromJSON: Composite 'NOT' is not an object.");
            return nullptr;
        }

        Condition* child = CreateConditionFromJSON(conditionsJSON["not"], conditionRegistry);
        if (!child)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionFactory.CreateConditionFromJSON: Failed child creation for NOT composition.");
            return nullptr;
        }

        return ENG_NEW(CompositeCondition, CompositeType::NOT, std::vector<Condition*>{child});
    }

    ConditionKey key = ConditionKey::ConditionKeyFromJSON(conditionsJSON);
    return conditionRegistry.GetOrCreateCondition(key);
}

Condition* ConditionFactory::CreateConditionFromKey(ConditionKey key)
{
    switch (key.type)
    {
        case ConditionType::Interact:
        {
            return ENG_NEW(InteractCondition);
        }

        case ConditionType::Stepped:
        {
            return ENG_NEW(SteppedOnCondition);
        }

        case ConditionType::Proximity:
        {
            if (!std::holds_alternative<uint64_t>(key.params))
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionFactory.CreateConditionFromKey: Proximity activator missing range parameter");
                return nullptr;
            }

            return ENG_NEW(ProximityCondition, std::get<uint64_t>(key.params));
        }

        case ConditionType::Facing:
        {
            if (!std::holds_alternative<FacingConditionType>(key.params))
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionFactory.CreateConditionFromKey: Facing activator missing facing parameter");
                return nullptr;
            }

            return ENG_NEW(FacingCondition, std::get<FacingConditionType>(key.params));
        }

        case ConditionType::Timed:
        {
            if (!std::holds_alternative<TimedConditionParams>(key.params))
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionFactory.CreateConditionFromKey: Timed activator missing timed parameters");
                return nullptr;
            }
            return ENG_NEW(TimedCondition, std::get<TimedConditionParams>(key.params));
		}

        case ConditionType::Cooldown:
        {
            if (!std::holds_alternative<uint64_t>(key.params))
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionFactory.CreateConditionFromKey: Cooldown activator missing time parameter");
                return nullptr;
            }
            return ENG_NEW(CooldownCondition, std::get<uint64_t>(key.params));
		}

        case ConditionType::MaxActivations:
        {
            if(!std::holds_alternative<uint64_t>(key.params))
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionFactory.CreateConditionFromKey: MaxActivations activator missing maxActivations parameter");
                return nullptr;
			}

			return ENG_NEW(MaxActivationsCondition, std::get<uint64_t>(key.params));
        }

        default:
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionFactory.CreateConditionFromJSON: Unknown activator type");
            return nullptr;
        }
    }
}
