#include <cmath>

#include "GuardTileMover.h"

#include "Configuration.h"
#include <SDL3/SDL.h>

#include "Guard.h"

#include "FloatUtils.h"
#include "GameMap.h"
#include "Unused.h"

GuardTileMover::GuardTileMover(float startX, float startY, Guard& guard) :
	TileMover(startX, startY),
	guard(guard) {}

GuardTileMover::GuardTileMover(const SDL_FPoint& position, Guard& guard) :
	GuardTileMover(position.x, position.y, guard) {}

void GuardTileMover::Update(double deltaTime)
{
    //SDL_Log("Guard TargetPoint: [%f], [%f]. CurrentPosition: [%f], [%f]", targetPosition.x, targetPosition.y, position.x, position.y);
	if (!isMovingFlag && directionProvider)
	{
		Direction next = directionProvider();
		if (!IsSentinel(next))
		{
			TryMove(next);
		}
		return;
	}

	MoveTowardsTarget(deltaTime);
}

void GuardTileMover::TryMove(Direction dir)
{
	if(IsSentinel(dir))
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GuardTileMover.TryMove: Invalid direction provided.");
		return;
	}

	if (isMovingFlag)
	{
		return;
	}

	currentDirection = dir;
    const GameConfiguration& config = Configuration::Get();
    const int tileWidth = config.tiles.width;
    const int tileHeight = config.tiles.height;
	SDL_FPoint newPosition = position;

	switch (dir)
	{
		case Direction::Up:
        {
            newPosition.y -= tileHeight;
            guard.SetSprite("GuardUp");
            break;
        }

		case Direction::Down:
        {
            newPosition.y += tileHeight;
            guard.SetSprite("GuardDown");
            break;
        }

		case Direction::Left:
        {
            newPosition.x -= tileWidth;
            guard.SetSprite("GuardLeft");
            break;
        }

		case Direction::Right:
        {
            newPosition.x += tileWidth;
            guard.SetSprite("GuardRight");
            break;
        }

        default:
        {
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GuardTileMover.TryMove: Invalid direction provided.");
			return;
        }
	}

    int tileX = static_cast<int>(newPosition.x) / tileWidth;
    int tileY = static_cast<int>(newPosition.y) / tileHeight;

    GameMap& map = GameMap::GetInstance();

    if (!map.IsInMapBounds(tileX, tileY) || map.IsTileSolid(tileX, tileY))
    {
        return;
    }

	targetPosition = newPosition;
	isMovingFlag = true;
}

void GuardTileMover::SetSpeed(float newSpeed) { speed = newSpeed; }

void GuardTileMover::SetDirectionProvider(const std::function<Direction()>& provider) { directionProvider = provider; }

Direction GuardTileMover::GetCurrentDirection() const { return currentDirection; }

void GuardTileMover::SetCurrentDirection(Direction newDirection) { currentDirection = newDirection; }

void GuardTileMover::MoveTowardsTarget(double deltaTime)
{
	float dx = targetPosition.x - position.x;
	float dy = targetPosition.y - position.y;

	const GameConfiguration& config = Configuration::Get();
	const int tileWidth = config.tiles.width;
	const int tileHeight = config.tiles.height;

	const float deltaTimeSeconds = static_cast<float>(deltaTime / 1000.0);

	const float activeTileDimension = (dx != 0.0f) ? static_cast<float>(tileWidth) : static_cast<float>(tileHeight);

	const float pixelsPerSecond = speed * activeTileDimension;
	const float moveStep = pixelsPerSecond * deltaTimeSeconds;

	const float distanceToTarget = std::max(std::abs(dx), std::abs(dy));

	if (distanceToTarget <= moveStep || distanceToTarget <= ConstantConfiguration::epsilon)
	{
		SetPosition(targetPosition);
		isMovingFlag = false;

		if (directionProvider)
		{
			Direction next = directionProvider();
			if (!IsSentinel(next)) 
			{
				TryMove(next);
			}
		}
		return;
	}

	const float angle = std::atan2(dy, dx);
	position.x += moveStep * std::cos(angle);
	position.y += moveStep * std::sin(angle);
}
