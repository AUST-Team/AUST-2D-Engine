#include <SDL3/SDL_log.h>

#include "FacingConditionType.h"

#include "StringMiscs.h"

FacingConditionType FacingConditionTypeFromString(const std::string& type)
{
	const std::string& typeLwr = StringMiscs::ToLower(type);

#define X(name, val, primary, alias)									\
    if (typeLwr == primary || (alias[0] != '\0' && typeLwr == alias))	\
        return FacingConditionType::name;

	FACING_CONDITION_TYPE_LIST(X)

#undef X

	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FacingConditionTypeFromString: Unknown activator type: %s. Defaulting to \"%s\"", type.c_str(), ConstantConfiguration::enumSentinelString);

	return GetSentinel<FacingConditionType>();;
}

std::string FacingConditionTypeToString(FacingConditionType type)
{
	switch (type)
	{
#define X(name, val, primary, alias)	\
        case FacingConditionType::name: return primary;

		FACING_CONDITION_TYPE_LIST(X)

#undef X

	default:
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "FacingConditionTypeToString: Invalid direction, defaulting to \"%s\"", ConstantConfiguration::enumSentinelStringLower);
			return ConstantConfiguration::enumSentinelStringLower;
		}
	}
}
