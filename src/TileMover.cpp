#include <cmath>
#include <SDL3/SDL_log.h>

#include "TileMover.h"

#include "Configuration.h"

TileMover::TileMover(float startX, float startY) :
	position({ startX, startY }),
	targetPosition(position),
	speed(Configuration::Get().character.speed) { SnapToTile(); }

TileMover::TileMover(const SDL_FPoint& position) :
	TileMover(position.x, position.y) {}

void TileMover::TryMove(Direction dir)
{
	if (isMovingFlag)
	{
		return;
	}

	const GameConfiguration& config = Configuration::Get();
	const int tileWidth = config.tiles.width;
	const int tileHeight = config.tiles.height;

	targetPosition = position;

	switch (dir)
	{
		case Direction::Up:
		{
			targetPosition.y -= tileHeight;
			break;
		}

		case Direction::Down:
		{
			targetPosition.y += tileHeight;
			break;
		}

		case Direction::Left:
		{
			targetPosition.x -= tileWidth;
			break;
		}

		case Direction::Right:
		{
			targetPosition.x += tileWidth;
			break;
		}

		default:
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TileMover.TryMove: Invalid direction.");
			return;
		}
	}

	isMovingFlag = true;
}

void TileMover::SnapToTile()
{
	const GameConfiguration& config = Configuration::Get();
	int tileWidth = config.tiles.width;
	int tileHeight = config.tiles.height;
	position.x = std::round(position.x / tileWidth) * tileWidth;
	position.y = std::round(position.y / tileHeight) * tileHeight;
}

void TileMover::SetPosition(float x, float y)
{
	position = { x, y };
	SnapToTile();
}

void TileMover::SetPosition(const SDL_FPoint& newPosition) { SetPosition(newPosition.x, newPosition.y); }

float TileMover::GetX() const { return position.x; }

float TileMover::GetY() const { return position.y; }

float TileMover::GetSpeed() const { return speed; }

bool TileMover::IsMoving() const { return isMovingFlag; }