#include <SDL3/SDL_log.h>

#include "PlayerTileMover.h"

#include "KeyManager.h"
#include "Configuration.h"
#include "Player.h"
#include "TileInstance.h"
#include "MoveCommand.h"
#include "GuardTileMover.h"
#include "FloatUtils.h"
#include "GameMap.h"
#include "TileRegistry.h"
#include "SDL_FPointOperators.h"  
#include "Unused.h"
#include "Distances.h"

PlayerTileMover::PlayerTileMover(float startX, float startY, Player& player) :
    TileMover(startX, startY),
    player(player), 
    keyManager(KeyManager::GetInstance()) { SnapToTile(); }

PlayerTileMover::PlayerTileMover(const SDL_FPoint& position, Player& player) :
    PlayerTileMover(position.x, position.y, player) {}

// No C++, I will never use [[maybe_unused]].
void PlayerTileMover::Update(double deltaTime)
{
    if (!isActiveFlag)
    {
        return;
    }

    if (isMovingFlag)
    {
        player.SetAnimationOverride(TileAnimationType::Walk);
        MoveTowardsTarget(deltaTime);
    }
    else
    {
        player.SetAnimationOverride(TileAnimationType::None);
        PollKeyManagerAndMove();
    }
}

void PlayerTileMover::TryMove(Direction dir)
{
    if (IsSentinel(dir))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "PlayerTileMover.TryMove: Invalid direction");
		return;
    }

    if (isMovingFlag)
    {
        queuedDirection = dir;
        return;
    }

    SDL_FPoint newPosition = position;

    const GameConfiguration& config = Configuration::Get();
    const int tileWidth = config.tiles.width;
    const int tileHeight = config.tiles.height;

    Reset(queuedDirection);

    if (!IsSentinel(lastDirection) && lastDirection != dir)
    {
        playerDirectionChangedFlag = true;
    }

    lastDirection = dir;

    switch (dir)
    {
        case Direction::Up:
        {
            newPosition.y -= tileHeight;
            break;
        }

        case Direction::Down:
        {
            newPosition.y += tileHeight;
            break;
        }

        case Direction::Left:
        {
            newPosition.x -= tileWidth;
            player.SetSprite("PlayerLeft");
            break;
        }

        case Direction::Right:
        {
            newPosition.x += tileWidth;
            player.SetSprite("PlayerRight");
            break;
        }

        default:
        {
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "PlayerTileMover.TryMove: Invalid direction");
            return;
        }
    }

    const int tileX = static_cast<int>(newPosition.x) / tileWidth;
    const int tileY = static_cast<int>(newPosition.y) / tileHeight;

    GameMap& map = GameMap::GetInstance();

    if (!map.IsInMapBounds(tileX, tileY) || map.IsTileSolid(tileX, tileY))
    {
        return; 
    }

    playerPositionChangedFlag = true;
    isMovingFlag = true;
    targetPosition = newPosition;
}

void PlayerTileMover::ResetMovementState()
{
    isMovingFlag = false;
	Reset(queuedDirection);
    targetPosition = position;
}

void PlayerTileMover::SetLastDirection(Direction newDirection) { lastDirection = newDirection; }

Direction PlayerTileMover::GetLastDirection() const { return lastDirection; }

void PlayerTileMover::SetActive(bool value) { isActiveFlag = value; }

void PlayerTileMover::MoveTowardsTarget(double deltaTime)
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

    if (distanceToTarget <= moveStep || distanceToTarget < ConstantConfiguration::epsilon)
    {
        SetPosition(targetPosition);

        GameMap& map = GameMap::GetInstance();
        map.NotifyPlayerMoved();

        const int tileX = static_cast<int>(position.x) / tileWidth;
        const int tileY = static_cast<int>(position.y) / tileHeight;

        if (playerDirectionChangedFlag || playerPositionChangedFlag)
        {
            map.NotifyPlayerChangedDirection();
            map.ApplyLookAnimation(tileX, tileY, lastDirection, OverrideType::Semi);
            map.ApplyLookSound(tileX, tileY, lastDirection);
            playerDirectionChangedFlag = false;
        }

        map.SetAnimationOverride(tileX, tileY, TileAnimationType::SteppedOn, OverrideType::Full);
        map.PlayAudioAt(tileX, tileY, TileSoundType::SteppedOn);

        isMovingFlag = false;

        if (!IsSentinel(queuedDirection) && IsDirectionHeld(queuedDirection))
        {
            TryMove(queuedDirection);
        }
        else
        {
            Reset(queuedDirection);
            PollKeyManagerAndMove();
        }
        return;
    }

    const float angle = std::atan2(dy, dx);
    position.x += moveStep * std::cos(angle);
    position.y += moveStep * std::sin(angle);
}

// We poll the key manager directly to get the held keysand move immediately so there's no delay between tiles.
void PlayerTileMover::PollKeyManagerAndMove()
{
    const std::vector<unsigned int>& heldKeys = keyManager.GetHeldKeys();
    if (!heldKeys.empty())
    {
        Direction dir = KeyToDirection(heldKeys.front());
        if (!IsSentinel(dir))
        {
            TryMove(dir);
        }
    }
}

bool PlayerTileMover::IsDirectionHeld(Direction dir)
{
    for (int keyCode : keyManager.GetHeldKeys())
    {
        Command* cmd = keyManager.GetCommandForKey(keyCode);
        if (auto moveCmd = dynamic_cast<MoveCommand*>(cmd))
        {
            if (moveCmd->GetDirection() == dir)
            {
                return true;
            }
        }
    }
    return false;
}

Direction PlayerTileMover::KeyToDirection(int keyCode)
{
    Command* cmd = keyManager.GetCommandForKey(keyCode);
    if (auto moveCmd = dynamic_cast<MoveCommand*>(cmd))
    {
        return moveCmd->GetDirection();
    }
    return GetSentinel<Direction>();;
}