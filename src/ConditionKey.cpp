#include <SDL3/SDL_log.h>

#include "ConditionKey.h"

#include "Miscs.h"
#include "JSONParser.h"

using json = nlohmann::json;

ConditionKey ConditionKey::ConditionKeyFromJSON(const json& conditionJSON)
{
	/*
		What this function expects (examples):

		"conditionJSON" here is an object from "conditions": { "or": [condition1, condition2, "and": [condition3, condition4], ...] }

		"conditionJSON": { "type": <conditionTypeName>, <optionalParameters>: ... }
		"conditionJSON": { "type": "interact" }
		"conditionJSON": { "type": "proximity", "range": 4 }
		"conditionJSON": { "type": "facing", "facing": "away" }
		"conditionJSON": { "type": "timed", "mode": "before", "time": 10.5 }
		"conditionJSON": { "type": "timed", "mode": "after", "time": 32 }
		"conditionJSON": { "type": "timed", "mode": "specific", "time": { "seconds": 1.2 } }
		"conditionJSON": { "type": "timed", "mode": "interval", "start": { "seconds": 1.2 }, "stop": { "ms": 5000 } }
		"conditionJSON": { "type": "cooldown", "time": 5 }
		"conditionJSON": { "type": "cooldown", "time": 3.25 }
		"conditionJSON": { "type": "cooldown", "time": { "seconds": 10 } }
		"conditionJSON": { "type": "maxActivations", "maxActivations": 5 }
	*/

	ConditionKey key;

	if (!conditionJSON.is_object())
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionKey.ConditionKeyFromJSON: Expected object, got non-object.");
		return key;
	}

	if (!conditionJSON.contains("type") || !conditionJSON["type"].is_string())
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionKey.ConditionKeyFromJSON: Condition type missing or not string.");
		return key;
	}

	const std::string& type = conditionJSON.at("type").get_ref<const std::string&>();
	ConditionType condType = ConditionTypeFromString(type);

	if (IsSentinel(condType))
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ConditionKey.ConditionKeyFromJSON: Condition type from string [%s] return sentinel value. Defaulting to 'Interact'.", type.c_str());
		condType = ConditionType::Interact;
	}

	key.type = condType;
	key.updateFlags = UpdateFlagsFromConditionType(key.type);
	key.params = ConditionParamsFromJSON(conditionJSON, key.type);

	return key;
}

ConditionParams ConditionKey::ConditionParamsFromJSON(const json& conditionJSON, ConditionType type)
{
	/*
		What this function expects (examples):

		"conditionJSON" here is an object from "conditions": { "or": [condition1, condition2, "and": [condition3, condition4], ...] }
		
		Note that the type is not used here, just the parameters, if any.

		"conditionJSON": { "type": <conditionTypeName>, <optionalParameters>: ... }
		"conditionJSON": { "type": "interact" }
		"conditionJSON": { "type": "proximity", "range": 4 }
		"conditionJSON": { "type": "facing", "facing": "away" }
		"conditionJSON": { "type": "timed", "mode": "before", "time": 10.5 }
		"conditionJSON": { "type": "timed", "mode": "after", "time": 32 }
		"conditionJSON": { "type": "timed", "mode": "specific", "time": { "seconds": 1.2 } }
		"conditionJSON": { "type": "timed", "mode": "interval", "start": { "seconds": 1.2 }, "stop": { "ms": 5000 } }
		"conditionJSON": { "type": "cooldown", "time": 5 }
		"conditionJSON": { "type": "cooldown", "time": 3.25 }
		"conditionJSON": { "type": "cooldown", "time": { "seconds": 10 } }
		"conditionJSON": { "type": "maxActivations", "maxActivations": 5 }
	*/

	if (!conditionJSON.is_object())
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionKey.ConditionParamsFromJSON: Expected object, got non-object.");
		return std::monostate{};
	}

	switch (type)
	{
		case ConditionType::Proximity:
		{
			if (!conditionJSON.contains("range") || !conditionJSON["range"].is_number())
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionKey.ParamsFromJSON: Range parameter missing or is not number for proximity activator.");
				return std::monostate{};
			}

			return conditionJSON.at("range").get<uint64_t>();
		}

		case ConditionType::Facing:
		{
			if (!conditionJSON.contains("facing") || !conditionJSON["facing"].is_string())
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionKey.ParamsFromJSON: Facing parameter missing or is not string for facing activator.");
				return std::monostate{};
			}

			const std::string& facing = conditionJSON.at("facing").get_ref<const std::string&>();
			FacingConditionType facingCondition = FacingConditionTypeFromString(facing);
			if (IsSentinel(facingCondition))
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionKey.ParamsFromJSON: Facing conditionJSON type from string [%s] return sentinel. Defaulting to 'LookAt'.", facing.c_str());
				return FacingConditionType::LookAt;
			}

			return facingCondition;
		}

		case ConditionType::Timed:
		{
			if (!conditionJSON.contains("mode") || !conditionJSON["mode"].is_string())
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionKey.ParamsFromJSON: Mode parameter missing or is not string for timed activator.");
				return std::monostate{};
			}

			const std::string& modeStr = conditionJSON.at("mode").get_ref<const std::string&>();
			TimedConditionType timedType = TimedConditionTypeFromString(modeStr);
			if (IsSentinel(timedType))
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionKey.ParamsFromJSON: Invalid timed mode [%s]; returned sentinel.", modeStr.c_str());
				return std::monostate{};
			}

			TimedConditionParams params;
			params.type = timedType;

			if (params.type == TimedConditionType::Interval)
			{
				if (!conditionJSON.contains("start") || !conditionJSON.contains("stop"))
				{
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionKey.ParamsFromJSON: Interval mode requires 'start' and 'stop' properties.");
					return std::monostate{};
				}
				params.startTime = JSONParser::ParseJSONTime<uint64_t>(conditionJSON["start"], 0ULL);
				params.endTime = JSONParser::ParseJSONTime<uint64_t>(conditionJSON["stop"], 0ULL);
			}
			else
			{
				if (!conditionJSON.contains("time"))
				{
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionKey.ParamsFromJSON: Timed mode requires a 'time' property.");
					return std::monostate{};
				}
				params.startTime = JSONParser::ParseJSONTime<uint64_t>(conditionJSON["time"], 0ULL);
				params.endTime = params.startTime;
			}

			return params;
		}

		case ConditionType::Cooldown:
		{
			if (!conditionJSON.contains("time"))
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionKey.ParamsFromJSON: Cooldown mode requires a 'time' property.");
				return std::monostate{};
			}
			uint64_t cooldownTime = JSONParser::ParseJSONTime<uint64_t>(conditionJSON["time"], 0ULL);
			return cooldownTime;
		}

		case ConditionType::MaxActivations:
		{
			if (!conditionJSON.contains("maxActivations") || !conditionJSON["maxActivations"].is_number_unsigned())
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionKey.ParamsFromJSON: MaxActivations mode requires a 'maxActivations' property of unsigned integer type.");
				return std::monostate{};
			}
			uint64_t maxActivations = conditionJSON.at("maxActivations").get<uint64_t>();
			return maxActivations;
		}

		case ConditionType::Interact:
		case ConditionType::Stepped:
		{
			return std::monostate{};
		}

		default:
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ConditionKey.ParamsFromJSON: Unknown condition type [%d | %s].", static_cast<int>(type), conditionJSON.at("type").get<std::string>().c_str());
			return std::monostate{};
		}
	}
}
