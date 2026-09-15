#include "Entity.h"

#include "Configuration.h"
#include "TileRegistry.h"

Entity::Entity() :
	dimensions(Size{ Configuration::Get().character.width, Configuration::Get().character.height }) {}

SDL_FPoint Entity::GetPosition() const { return SDL_FPoint{ GetX(), GetY() }; }

int Entity::GetTileX() const { return (static_cast<int> (GetX() / Configuration::Get().tiles.width)); }

int Entity::GetTileY() const { return (static_cast<int> (GetY() / Configuration::Get().tiles.height)); }

SDL_Point Entity::GetTilePosition() const
{
	const GameConfiguration& config = Configuration::Get();
	const int tileWidth = config.tiles.width;
	const int tileHeight = config.tiles.height;

	return SDL_Point {
		static_cast<int>(std::round(GetX() / tileWidth)),
		static_cast<int>(std::round(GetY() / tileHeight))
	};
}

int Entity::GetWidth() const { return dimensions.w; }

int Entity::GetHeight() const { return dimensions.h; }

Size Entity::GetSize() const { return dimensions; }

float Entity::GetSpeed() const { return Configuration::Get().character.speed; }

void Entity::SetSprite(int newSprite) { sprite = newSprite; }

void Entity::SetSprite(const std::string& newSprite) { sprite = TileRegistry::GetTileIndexByName(newSprite); }

int Entity::GetSprite() const { return sprite; }


