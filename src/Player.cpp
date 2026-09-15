#include <stdexcept>
#include <SDL3/SDL.h>

#include "Player.h"

#include "Configuration.h"
#include "Camera.h"
#include "Assets.h"
#include "Observer.h"
#include "GameMap.h"
#include "Direction.h"
#include "PlayerStrategyManager.h"
#include "TileRegistry.h"
#include "Interactable.h"
#include "MemoryTracker.h"
#include "Miscs.h"
#include "StringMiscs.h"
#include "DataProvider.h"

Player* Player::playerInstance = nullptr;
bool Player::created = false;

Player::Player(int startX, int startY) :
    tileMover(
        static_cast<float>(startX) * Configuration::Get().tiles.width,
        static_cast<float>(startY) * Configuration::Get().tiles.height,
        *this
    ),
    animationOverride(TileAnimationType::None) 
{
    sprite = TileRegistry::GetTileIndexByName("PlayerLeft");
}

Player::~Player() { ClearObservers(); }

Player* Player::CreateInstance(int startX, int startY)
{
    if (!playerInstance && !created)
    {
        created = true;
        playerInstance = ENG_NEW(Player, startX, startY);

        DataProvider::GetInstance().RegisterProvider(ConstantConfiguration::playerCaughtDataKey, []() {
            return playerInstance->timesCaught;
        });
    }

    return playerInstance;
}

Player* Player::CreateInstance(const SDL_Point& startPoint) { return CreateInstance(startPoint.x, startPoint.y); }

Player* Player::GetInstance() { return playerInstance; }

void Player::DeleteInstance()
{
    if (playerInstance && created)
    {
		const int caughtCount = playerInstance->GetTimesCaught();
        DataProvider::GetInstance().RegisterProvider(ConstantConfiguration::playerCaughtDataKey, [caughtCount]() {
            return caughtCount;
        });

        playerInstance->ClearObservers();
        ENG_DELETE(playerInstance);
        created = false;
        playerInstance = nullptr;
    }
    else
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Player.DeleteInstance: Player instance already deleted or doesn't exist.");
    }
}

void Player::Update(double deltaTime) 
{
    tileMover.Update(deltaTime);

    PlayerStrategyManager::Update(deltaTime);

    for (Observer* o : observers)
    {
        if (o)
        {
            o->Update(deltaTime, *this);
        }
    }
}

void Player::AddObserver(Observer* o) { observers.push_back(o); }

void Player::RemoveObserver(Observer* o) { observers.erase(std::remove(observers.begin(), observers.end(), o), observers.end()); }

void Player::ClearObservers() { observers.clear(); }

void Player::SetPosition(float x, float y) { tileMover.SetPosition(x, y); }

void Player::SetPosition(const SDL_FPoint& newPosition) { tileMover.SetPosition(newPosition); }

void Player::SetPositionTile(int x, int y) 
{ 
    const GameConfiguration& config = Configuration::Get();
    const int tileWidth = config.tiles.width;
    const int tileHeight = config.tiles.height;

    tileMover.SetPosition(
        static_cast<float>(x) * tileWidth,
        static_cast<float>(y) * tileHeight
    ); 
}

void Player::SetPositionTile(const SDL_Point& newPosition) { SetPositionTile(newPosition.x, newPosition.y); }

void Player::ResetMovementState() { tileMover.ResetMovementState(); }

float Player::GetX() const { return tileMover.GetX(); }

float Player::GetY() const { return tileMover.GetY(); }

SDL_FPoint Player::GetPosition() const { return SDL_FPoint{ tileMover.GetX(), tileMover.GetY() }; }

float Player::GetSpeed() const { return tileMover.GetSpeed(); }

int Player::GetTileX() const { return (static_cast<int> (tileMover.GetX() / Configuration::Get().tiles.width)); }

int Player::GetTileY() const { return (static_cast<int> (tileMover.GetY() / Configuration::Get().tiles.height)); }

SDL_Point Player::GetTilePosition() const
{
    const GameConfiguration& config = Configuration::Get();
    const int tileWidth = config.tiles.width;
    const int tileHeight = config.tiles.height;

    return SDL_Point {
        static_cast<int> (tileMover.GetX() / tileWidth),
        static_cast<int> (tileMover.GetY() / tileHeight)
    };
}

bool Player::IsMoving() const { return tileMover.IsMoving(); }

void Player::SetLastDirection(Direction newDirection)
{
    if (IsSentinel(newDirection))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Player.SetLastDirection: Attempted to set a null direction. Defaulting to 'Right'");
        newDirection = Direction::Right;
    }

    std::string tile = "Player";

    const std::string& directionStr = DirectionToString(newDirection);

    tile += StringMiscs::IsStringEnumSentinel(directionStr) ? "Right" : StringMiscs::CapitalizeFirst(directionStr);

    sprite = TileRegistry::GetTileIndexByName(tile);
    tileMover.SetLastDirection(newDirection);
}

Direction Player::GetLastDirection() const { return tileMover.GetLastDirection(); }

int Player::GetTimesCaught() const { return timesCaught; }

void Player::IncrementTimesCaught() { SetTimesCaught(timesCaught + 1); }

void Player::SetTimesCaught(int newValue) 
{ 
    timesCaught = newValue; 
    const int caughtCount = timesCaught;
    DataProvider::GetInstance().RegisterProvider(ConstantConfiguration::playerCaughtDataKey, [caughtCount]() {
        return caughtCount;
    });
}

void Player::TryMove(Direction dir) { tileMover.TryMove(dir); }

void Player::SetActiveMovement(bool value) { tileMover.SetActive(value); }

void Player::SetCurrentStrategy(PlayerStrategy* newStrategy) { currentStrategy = newStrategy; }

PlayerStrategy* Player::GetCurrentStrategy() { return currentStrategy; }

void Player::SetAnimationOverride(TileAnimationType override) { animationOverride = override; }

TileAnimationType Player::GetAnimationOverride() const { return animationOverride; }
