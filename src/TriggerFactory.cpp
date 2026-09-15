#include <SDL3/SDL_log.h>

#include "TriggerFactory.h"

#include "Trigger.h"
#include "InteractableFactory.h"
#include "ConditionFactory.h"
#include "ActivationExpression.h"
#include "MemoryTracker.h"
#include "JSONParser.h"

using json = nlohmann::json;

Trigger* TriggerFactory::CreateTriggerFromJSON(const json& triggerJSON, ConditionRegistry& conditionRegistry)
{
	/*
		What this function expects (example):
		
		"triggerJSON" here is one of the objects from the "triggers": [trigger1, trigger2, ...]

		{
			"name": "triggerName"
			"position": { "x": 11, "y": 11 },

			"flags": {
				"repeatable": true,
				"autoDelete": false,
			},

			"conditions": {
				"or": [
					{"type": "interact"},
					{"type": "steppedOn"},
					{"and": [
						{"type": "proximity", "range": 4},
						{"type": "facing", "facing": "away"}
					]}
				]
			},

			"events": {
				"startDialogue": {
					"type": "narration",
					"lines": [ "First Line", "Second Line" ],
					"onEnd": [<events>] or NULL
				}
			}
		}
	*/

	if (!triggerJSON.is_object())
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TriggerFactory.CreateTriggerFromJSON: Expected object, got non-object.");
		return nullptr;
	}

	if (!triggerJSON.contains("name") || !triggerJSON["name"].is_string())
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TriggerFactory.CreateTriggerFromJSON: Trigger name missing or is not an string.");
		return nullptr;
	}

	const std::string& name = triggerJSON.at("name").get_ref<const std::string&>();

	if (!triggerJSON.contains("flags") || !triggerJSON["flags"].is_object())
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TriggerFactory.CreateTriggerFromJSON: Trigger flags missing or is not an object for id [%s].", name.c_str());
		return nullptr;
	}

	if (!triggerJSON.contains("conditions") || !triggerJSON["conditions"].is_object())
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TriggerFactory.CreateTriggerFromJSON: Trigger conditions missing or is not an object for id [%s].", name.c_str());
		return nullptr;
	}

	if (!triggerJSON.contains("events") || (!triggerJSON["events"].is_object() && !triggerJSON["events"].is_array()))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TriggerFactory.CreateTriggerFromJSON: Trigger events missing or is not an object / array for id [%s].", name.c_str());
		return nullptr;
	}

	if (!triggerJSON.contains("position") || !triggerJSON["position"].is_object())
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TriggerFactory.CreateTriggerFromJSON: Trigger position missing or is not an object for id [%s]. Attemping to default to {0, 0}", name.c_str());
	}

	const json pos = triggerJSON.value("position", json::object());
	const SDL_Point position = JSONParser::ParseJSONPoint<SDL_Point>(pos, SDL_Point{ 0, 0 });

	const json& flags = triggerJSON["flags"];
	const json& conditions = triggerJSON["conditions"];
	const json& events = triggerJSON["events"];

	std::vector<SystemAction> eventsVec = JSONParser::ParseJSONSystemActions(events);

	if (eventsVec.empty())
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TriggerFactory.CreateTriggerFromJSON: Events parsed returned empty vector for triggerJSON at [x = %d, y = %d] for id [%s]", position.x, position.y, name.c_str());
		return nullptr;
	}

	Condition* conditionPtr = ConditionFactory::CreateConditionFromJSON(conditions, conditionRegistry);

	if (!conditionPtr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TriggerFactory.CreateTriggerFromJSON: Condition factory created a null condition for triggerJSON at [x = %d, y = %d] for id [%s]", position.x, position.y, name.c_str());
		return nullptr;
	}

	ActivationExpression* activationExpressionPtr = ENG_NEW(ActivationExpression, conditionPtr);
	Trigger* triggerPtr = ENG_NEW(Trigger, name, position, eventsVec, activationExpressionPtr, TriggerFlags::None);

	SetTriggerFlagsFromJSON(flags, triggerPtr);

	return triggerPtr;
}

void TriggerFactory::SetTriggerFlagsFromJSON(const json& flagsJSON, Trigger* trigger)
{
	/*
		What this function expects:

		Note that 'flagJSON' is the main JSON object.

		{
			"repeatable": true,
			"autoDelete": false,
			"triggered": false,
		},
	*/

	if (!flagsJSON.is_object())
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TriggerFactory.SetTriggerFlagsFromJSON: Expected object, got non-object.");
		return;
	}

	if (!trigger)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TriggerFactory.SetTriggerFlagsFromJSON: Passed null triggerJSON.");
		return;
	}

	TriggerFlags triggerFlags = TriggerFlags::None;
	if (flagsJSON.value("repeatable", false))
	{
		triggerFlags |= TriggerFlags::Repeatable;
	}
	if (flagsJSON.value("autoDelete", true))
	{
		triggerFlags |= TriggerFlags::AutoDelete;
	}
	if (flagsJSON.value("triggered", false))
	{
		trigger->SetTriggeredState(true);
	}

	trigger->SetTriggerFlags(triggerFlags);
}
