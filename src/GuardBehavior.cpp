#include "GuardBehavior.h"

#include "Guard.h"
#include "GuardPathStrategy.h"
#include "GuardPatrolStrategy.h"
#include "MemoryTracker.h"

GuardAIData::GuardAIData(int targetX, int targetY) :
	type(GuardBehaviorType::IdleReturn)
{
	data.moveToPointStrategy = ENG_NEW(GuardPathStrategy, targetX, targetY);
}

GuardAIData::GuardAIData(const SDL_Point& targetPoint) :
	GuardAIData(targetPoint.x, targetPoint.y) {}

GuardAIData::GuardAIData(const std::vector<SDL_Point>& patrolPoints) :
	type(GuardBehaviorType::Patrol)
{
	data.patrolStrategy = ENG_NEW(GuardPatrolStrategy, patrolPoints);
}

GuardAIData::GuardAIData(const GuardAIData& other) :
	type(other.type)
{
	switch (type)
	{
		case GuardBehaviorType::IdleReturn:
		{
			data.moveToPointStrategy = ENG_NEW(GuardPathStrategy, *other.data.moveToPointStrategy);
			break;
		}

		case GuardBehaviorType::Patrol:
		{
			data.patrolStrategy = ENG_NEW(GuardPatrolStrategy, *other.data.patrolStrategy);
			break;
		}

		default:
		{
			std::memset(&data, 0, sizeof(data));
			break;
		}
	}
}

GuardAIData::GuardAIData(GuardAIData&& other) noexcept :
	type(other.type)
{
	switch (type)
	{
		case GuardBehaviorType::IdleReturn:
		{
			data.moveToPointStrategy = other.data.moveToPointStrategy;
			other.data.moveToPointStrategy = nullptr;
			break;
		}

		case GuardBehaviorType::Patrol:
		{
			data.patrolStrategy = other.data.patrolStrategy;
			other.data.patrolStrategy = nullptr;
			break;
		}

		default:
		{
			std::memset(&data, 0, sizeof(data));
			break;
		}
	}
}

GuardAIData::~GuardAIData()
{
	switch (type)
	{
		case GuardBehaviorType::IdleReturn:
		{
			ENG_DELETE(data.moveToPointStrategy);
			data.moveToPointStrategy = nullptr;
			break;
		}

		case GuardBehaviorType::Patrol:
		{
			ENG_DELETE(data.patrolStrategy);
			data.patrolStrategy = nullptr;
			break;
		}
	}

	std::memset(&data, 0, sizeof(data));
}

GuardAIData& GuardAIData::operator=(const GuardAIData& other)
{
	if (this == &other)
		return *this;

	this->~GuardAIData();

	type = other.type;

	switch (type)
	{
		case GuardBehaviorType::IdleReturn:
		{
			data.moveToPointStrategy = ENG_NEW(GuardPathStrategy, *other.data.moveToPointStrategy);
			break;
		}

		case GuardBehaviorType::Patrol:
		{
			data.patrolStrategy = ENG_NEW(GuardPatrolStrategy, *other.data.patrolStrategy);
			break;
		}

		default:
		{
			std::memset(&data, 0, sizeof(data));
			break;
		}
	}

	return *this;
}

GuardAIData& GuardAIData::operator=(GuardAIData&& other) noexcept
{
	if (this == &other)
	{
		return *this;
	}

	this->~GuardAIData();

	type = other.type;

	switch (type)
	{
		case GuardBehaviorType::IdleReturn:
		{
			data.moveToPointStrategy = other.data.moveToPointStrategy;
			other.data.moveToPointStrategy = nullptr;
			break;
		}

		case GuardBehaviorType::Patrol:
		{
			data.patrolStrategy = other.data.patrolStrategy;
			other.data.patrolStrategy = nullptr;
			break;
		}

		default:
		{
			std::memset(&data, 0, sizeof(data));
			break;
		}
	}

	return *this;
}
