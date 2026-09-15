#include <SDL3/SDL_log.h>

#include "TimedConditionType.h"

#include "StringMiscs.h"

TimedConditionType TimedConditionTypeFromString(const std::string& type)
{
	const std::string& typeLwr = StringMiscs::ToLower(type);

#define X(name, val, str)					\
    if (typeLwr == str)						\
        return TimedConditionType::name;

	TIMED_CONDITION_TYPE_LIST(X)

#undef X

	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TimedConditionTypeFromString: Unknown activator type: %s. Defaulting to \"%s\"", type.c_str(), ConstantConfiguration::enumSentinelString);

    return GetSentinel<TimedConditionType>();
}

std::string TimedConditionTypeToString(TimedConditionType type)
{
    switch (type)
    {
#define X(name, val, str) \
        case TimedConditionType::name: return str;

        TIMED_CONDITION_TYPE_LIST(X)

#undef X

        default:
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TimedConditionTypeToString: Unknown timed condition type. Defaulting to \"%s\"", ConstantConfiguration::enumSentinelStringLower);
            return ConstantConfiguration::enumSentinelStringLower;
        }
    }
}
