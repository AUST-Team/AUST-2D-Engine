#include <SDL3/SDL_log.h>

#include "Direction.h"

#include "FloatUtils.h"
#include "StringMiscs.h"

Direction GetDirectionTo(float dx, float dy)
{
	if (FloatUtils::IsZeroF(dx * dx + dy * dy))
	{
		return GetSentinel<Direction>();
	}

	if (FloatUtils::IsGreaterAbsF(dx, dy))
	{
		return dx > 0 ? Direction::Right : Direction::Left;
	}
	else
	{
		return dy > 0 ? Direction::Down : Direction::Up;
	}
}

Direction GetOppositeDirection(Direction dir)
{
	switch (dir)
	{
		case GetSentinel<Direction>():	return GetSentinel<Direction>();
		case Direction::Up:			return Direction::Down;
		case Direction::Down:		return Direction::Up;
		case Direction::Left:		return Direction::Right;
		case Direction::Right:		return Direction::Left;
	}

	SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GetOppositeDirection: Invalid direction. Defaulting to \"%s\"", ConstantConfiguration::enumSentinelString);
	return GetSentinel<Direction>();
}

Direction DirectionFromString(const std::string& directionName)
{
	const std::string& nameLwr = StringMiscs::ToLower(directionName);

#define X(name, val, str)	\
    if (nameLwr == str)		\
        return Direction::name;

	DIRECTION_LIST(X)

#undef X

	SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DirectionFromString: Invalid direction name [%s]. Defaulting to \"%s\"", directionName.c_str(), ConstantConfiguration::enumSentinelString);
	return GetSentinel<Direction>();;
}

std::string DirectionToString(Direction dir)
{
	switch (dir)
	{
#define X(name, val, str) \
        case Direction::name: return str;

		DIRECTION_LIST(X)

#undef X

		default:
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DirectionToString: Invalid direction, defaulting to \"%s\"", ConstantConfiguration::enumSentinelStringLower);
			return ConstantConfiguration::enumSentinelStringLower;
		}
	}
}
