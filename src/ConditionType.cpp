#include <unordered_map>
#include <SDL3/SDL_log.h>

#include "ConditionType.h"

#include "StringMiscs.h"

ConditionType ConditionTypeFromString(const std::string& type)
{
	const std::string& typeLwr = StringMiscs::ToLower(type);

#define X(name, val, str, flag)	\
    if (typeLwr == str)			\
        return ConditionType::name;

	CONDITION_TYPE_LIST(X)

#undef X

	if (typeLwr == "steppedon")
	{
		return ConditionType::Stepped;
	}

	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ConditionTypeFromString: Unknown activator type: %s. Defaulting to \"%s\"", type.c_str(), ConstantConfiguration::enumSentinelString);
	return GetSentinel<ConditionType>();
}

std::string ConditionTypeToString(ConditionType type)
{
	switch (type)
	{
#define X(name, val, str, flag)	\
        case ConditionType::name: return str;

		CONDITION_TYPE_LIST(X)

#undef X

		default:
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "ConditionTypeToString: Unknown condition type. Defaulting to \"%s\"", ConstantConfiguration::enumSentinelStringLower);
			return ConstantConfiguration::enumSentinelStringLower;
		}
	}
}

MapUpdateFlags UpdateFlagsFromConditionType(ConditionType type)
{
	switch (type)
	{
#define X(name, val, str, flag) case ConditionType::name: return flag;
		CONDITION_TYPE_LIST(X)
#undef X
		default:
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UpdateFlagsFromConditionType: Unknown condition type. Defaulting to \"None\"");
			return MapUpdateFlags::None;
		}
	}
}
