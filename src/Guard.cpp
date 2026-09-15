#include <SDL3/SDL.h>
#include <algorithm>

#include "Guard.h"

#include "Player.h"
#include "Configuration.h"
#include "GuardStateManager.h"
#include "Camera.h"
#include "Assets.h"
#include "GuardState.h"
#include "PlayerStrategyManager.h"
#include "TileRegistry.h"
#include "GuardManager.h"
#include "GameMap.h"
#include "FlagManager.h"
#include "VectorUtils.h"
#include "MemoryTracker.h"
#include "StringMiscs.h"
#include "SettingsManager.h"

Player* Guard::player = nullptr;
int Guard::guardCount = 0;

Guard::Guard(int startX, int startY, Direction facingDirection) :
	tileMover(
		static_cast<float>(startX) * Configuration::Get().tiles.width,
		static_cast<float>(startY) * Configuration::Get().tiles.height,
		*this),
	AIData(GetTileX(), GetTileY()) { Init(facingDirection); }

Guard::Guard(const SDL_Point& spawnPoint, Direction facingDirection) :
	Guard(spawnPoint.x, spawnPoint.y, facingDirection) {}

Guard::Guard(int startX, int startY, const std::vector<SDL_Point>& patrolPoints, Direction facingDirection) :
	tileMover(
		static_cast<float>(startX) * Configuration::Get().tiles.width,
		static_cast<float>(startY) * Configuration::Get().tiles.height,
		*this), 
	AIData(patrolPoints) { Init(facingDirection); }

Guard::Guard(const SDL_Point& spawnPoint, const std::vector<SDL_Point>& patrolPoints, Direction facingDirection) :
	Guard(spawnPoint.x, spawnPoint.y, patrolPoints, facingDirection) {}

// Oh god.
Guard::Guard(Guard&& other) noexcept :
	tileMover(std::move(other.tileMover)),
	AIData(std::move(other.AIData)),
	currentState(std::exchange(other.currentState, nullptr)),
	visionRange(std::move(other.visionRange)),
	visionReductionElapsed(other.visionReductionElapsed),
	startVisionRange(std::move(other.startVisionRange)),
	aggressionLevel(other.aggressionLevel),
	aggressionReductionElapsed(other.aggressionReductionElapsed),
	alertSprite(other.alertSprite),
	alertStartTime(other.alertStartTime),
	chaseStartTime(other.chaseStartTime),
	alertIconElapsedTime(other.alertIconElapsedTime) {}

Guard::~Guard()
{
	--guardCount;
	if (guardCount == 0)
	{
		// Non-owning pointer, DON'T DELETE!!!
		player = nullptr;
	}

	if (currentState && !currentState->IsShared())
	{
		ENG_DELETE(currentState);
		currentState = nullptr;
	}
}

void Guard::Update(double deltaTime)
{
	if (isReducingAggressionFlag)
	{
		UpdateAggression(deltaTime);
	}
	
	if (isReducingVisionFlag)
	{
		UpdateVision(deltaTime);
	}

	tileMover.Update(deltaTime);

	GuardStateManager::Update(deltaTime, *this);
}

bool Guard::CanSeePlayer()
{
	return !SettingsManager::GetInstance().ShouldPlayerBeInvisible() && IsPlayerInVisionCone() && HasLineOfSightToPlayer() && (PlayerStrategyManager::GetCurrentStrategyType() != PlayerStrategyType::Caught);
}

void Guard::SetPosition(float x, float y) { tileMover.SetPosition(x, y); }

void Guard::SetPosition(const SDL_FPoint& newPosition) { tileMover.SetPosition(newPosition); }

void Guard::SetPositionTile(int x, int y) 
{ 
	const GameConfiguration& config = Configuration::Get();
	const int tileWidth = config.tiles.width;
	const int tileHeight = config.tiles.height;

	tileMover.SetPosition(
		static_cast<float>(x) * tileWidth,
		static_cast<float>(y) * tileHeight
	); 
}

void Guard::SetPositionTile(const SDL_Point& newPosition) { SetPositionTile(newPosition.x, newPosition.y); }

float Guard::GetSpeed() const { return tileMover.GetSpeed(); }

void Guard::SetSpeed(float speed) { tileMover.SetSpeed(speed); }

float Guard::GetX() const { return tileMover.GetX(); }

float Guard::GetY() const { return tileMover.GetY(); }

SDL_FPoint Guard::GetPosition() const { return SDL_FPoint { tileMover.GetX(), tileMover.GetY() }; }

int Guard::GetTileX() const { return (static_cast<int> (tileMover.GetX() / Configuration::Get().tiles.width)); }

int Guard::GetTileY() const { return (static_cast<int> (tileMover.GetY() / Configuration::Get().tiles.height)); }

SDL_Point Guard::GetTilePosition() const
{
	const GameConfiguration& config = Configuration::Get();
	const int tileWidth = config.tiles.width;
	const int tileHeight = config.tiles.height;

	return SDL_Point {
		static_cast<int> (tileMover.GetX() / tileWidth),
		static_cast<int> (tileMover.GetY() / tileHeight)
	};
}

bool Guard::IsMoving() const { return tileMover.IsMoving(); }

void Guard::SetDirectionProvider(const std::function<Direction()>& provider) { tileMover.SetDirectionProvider(provider); }

GuardState* Guard::GetCurrentState() const { return currentState; }

void Guard::SetCurrentState(GuardState* newState) { currentState = newState; }

const GuardAIData& Guard::GetAIData() const { return AIData; }

void Guard::SetVisionRange(const VisionRange& range) 
{ 
	visionRange = range;
	isReducingVisionFlag = false;
}

void Guard::StartVisionDecrease()
{
	if (isReducingVisionFlag)
	{
		return;
	}

	startVisionRange = visionRange;

	visionReductionElapsed = 0.f;
	isReducingVisionFlag = true;
}

void Guard::UpdateVision(double deltaTime)
{
	if (!isReducingVisionFlag)
	{
		return;
	}

	const GameConfiguration& config = Configuration::Get();
	const float visionReductionDuration = config.guard.timings.visionReduction;
	const VisionRangeModifierConfiguration& patrolVisionRangeMod = config.guard.vision.patrol;

	VisionRange patrolVisionRange = config.guard.MakeVisionRange(patrolVisionRangeMod);

	visionReductionElapsed += static_cast<float>(deltaTime);
	float t = visionReductionElapsed / visionReductionDuration;

	if (t > 1.0) 
	{
		t = 1.0;
	}

	// Interpolation my beloved.
	visionRange.tileRanges.centralTileRange = static_cast<int>(
		std::lerp(static_cast<double>(startVisionRange.tileRanges.centralTileRange),
			static_cast<double>(patrolVisionRange.tileRanges.centralTileRange),
			t)
	);

	visionRange.tileRanges.normalTileRange = static_cast<int>(
		std::lerp(static_cast<double>(startVisionRange.tileRanges.normalTileRange),
			static_cast<double>(patrolVisionRange.tileRanges.normalTileRange),
			t)
	);

	visionRange.tileRanges.peripheralTileRange = static_cast<int>(
		std::lerp(static_cast<double>(startVisionRange.tileRanges.peripheralTileRange),
			static_cast<double>(patrolVisionRange.tileRanges.peripheralTileRange),
			t)
	);

	visionRange.coneAngles.centralConeAngle = static_cast<float>(
		std::lerp(static_cast<double>(startVisionRange.coneAngles.centralConeAngle),
			static_cast<double>(patrolVisionRange.coneAngles.centralConeAngle),
			t)
	);

	visionRange.coneAngles.normalConeAngle = static_cast<float>(
		std::lerp(static_cast<double>(startVisionRange.coneAngles.normalConeAngle),
			static_cast<double>(patrolVisionRange.coneAngles.normalConeAngle),
			t)
	);

	visionRange.coneAngles.peripheralConeAngle = static_cast<float>(
		std::lerp(static_cast<double>(startVisionRange.coneAngles.peripheralConeAngle),
			static_cast<double>(patrolVisionRange.coneAngles.peripheralConeAngle),
			t)
	);

	if (t >= 1.0)
	{
		visionRange = patrolVisionRange;
		isReducingVisionFlag = false;
	}
}

float Guard::GetAggressionLevel() const { return aggressionLevel; }

void Guard::SetAggression(float newAggression) 
{ 
	aggressionLevel = std::clamp(newAggression, 0.0f, 1.0f);
	isReducingAggressionFlag = false;
}

void Guard::AddAggressionBasedOnDistance(float distance, double deltaTime)
{
	isReducingAggressionFlag = false;

	double deltaTimeSec = deltaTime / 1000.0;

	constexpr float aggressionSpeedFactor = 2.0f; // Tweak to control max gain rate.
	constexpr float distanceOffset = 0.5f;        // Prevents huge aggression at > 1 distance (if it ever gets there).
	constexpr float distanceExponent = 0.5f;      // > 1.0 means fast falloff with distance.

	float denominator = std::pow(distance + distanceOffset, distanceExponent);
	float deltaAggro = aggressionSpeedFactor * static_cast<float>(deltaTimeSec) / denominator;

	aggressionLevel = std::clamp(aggressionLevel + deltaAggro, 0.0f, 1.0f);
}

void Guard::StartAggressionDecrease()
{
	if (aggressionLevel <= 0.0f || isReducingAggressionFlag)
	{
		return;
	}

	aggressionReductionElapsed = 0.0f;
	isReducingAggressionFlag = true;
}

void Guard::UpdateAggression(double deltaTime)
{
	if (!isReducingAggressionFlag)
	{
		return;
	}

	aggressionReductionElapsed += static_cast<float>(deltaTime);
	float progress = aggressionReductionElapsed / Configuration::Get().guard.timings.aggressionReduction;

	aggressionLevel = std::max(0.0f, 1.0f - progress);

	if (aggressionLevel <= 0.0f)
	{
		aggressionLevel = 0.0f;
		isReducingAggressionFlag = false;
	}
}

void Guard::ShowChaseAlertIcon()
{
	alertSprite = TileRegistry::GetTileIndexByName("ChaseIcon");
	showAlertFlag = true;
	alertStartTime = SDL_GetTicks();
}

void Guard::ShowSearchAlertIcon()
{
	alertSprite = TileRegistry::GetTileIndexByName("SearchIcon");
	showAlertFlag = true;
	alertStartTime = SDL_GetTicks();
}

void Guard::SetChaseStartTime(uint64_t time) { chaseStartTime = time; }

uint64_t Guard::GetChaseStartTime() const { return chaseStartTime; }

void Guard::SetChaseAlertFlag(bool value) { showChaseAlertFlag = value; }

bool Guard::CanShowChaseAlert() const { return showChaseAlertFlag; }

bool Guard::IsAlertActive() const { return showAlertFlag; }

void Guard::SetAlertActive(bool value) { showAlertFlag = value; }

uint64_t Guard::GetAlertStartTime() const { return alertStartTime; }

int Guard::GetAlertTileSprite() const { return alertSprite; }

float Guard::GetTimeSinceLastPlayerSeen() const { return timeSinceLastPlayerSeen; }

void Guard::SetTimeSinceLastPlayerSeen(float newTime) { timeSinceLastPlayerSeen = newTime; }

void Guard::AddTimeSinceLastPlayerSeen(float addTime) { timeSinceLastPlayerSeen += addTime; }

void Guard::SetCaughtDialogueData(const DialogueData& data) { caughtDialogueData = data; }

std::optional<DialogueData> Guard::GetCaughtDialogueData() const { return caughtDialogueData; }

bool Guard::HasLineOfSightToPlayer() const
{
	int x0 = GetTileX();
	int y0 = GetTileY();
	int x1 = player->GetTileX();
	int y1 = player->GetTileY();

	GameMap& map = GameMap::GetInstance();

	// This uses Bresenham's line algorithm.
	int dx = abs(x1 - x0);	// Difference along the X axis.
	int dy = abs(y1 - y0);	// Difference along the Y axis.
	int sx = (x0 < x1) ? 1 : -1;	// Step to take along the X axis (1 - right, -1 - left).
	int sy = (y0 < y1) ? 1 : -1;	// Step to take along the Y axis (1 - down, -1 - up).
	int err = dx - dy;	// Initial 'error' value; used to determine which tile is 'closer' to line from point [x0, y0] to point [x1, y1]. Used in determining in which axis to step next in to.

	while (true)
	{
		// Check if the tile is in the map bounds or if it blocks sight.
		if (!map.IsInMapBounds(x0, y0) || map.IsTileBlockingVision(x0, y0))
		{
			// If yes, no line of sight.
			return false;
		}

		// Check if reached target tile (player).
		if (x0 == x1 && y0 == y1)
		{
			// If yes, break the loop - there is line of sight.
			break;
		}

		// Decide which axis to move on to next using a scaled 'error' value.
		int e2 = 2 * err;

		// Move on the X axis if the error allows it.
		if (e2 > -dy) 
		{
			err -= dy;
			x0 += sx;
		}

		// Move on the Y axis if the error allows it.
		if (e2 < dx) 
		{
			err += dx;
			y0 += sy;
		}
	}

	// There is line of sight.
	return true;
}

bool Guard::IsPlayerInVisionCone() const
{
	const float guardX = GetX();
	const float guardY = GetY();
	const float playerX = player->GetX();
	const float playerY = player->GetY();

	const float dx = playerX - guardX;
	const float dy = playerY - guardY;
	const float distance = std::sqrt(dx * dx + dy * dy);

	const GameConfiguration& config = Configuration::Get();
	const int tileWidth = config.tiles.width;
	const int tileHeight = config.tiles.height;

	if (std::abs(dx) <= static_cast<float>(tileWidth) &&
		std::abs(dy) <= static_cast<float>(tileHeight))
	{
		return true;
	}

	std::optional<Direction> facing = tileMover.GetCurrentDirection();
	if (!facing)
	{
		return false;
	}

	SDL_FPoint facingDir = GetDirectionOffsetFloat(*facing);
	SDL_FPoint toPlayer = VectorUtils::Normalize(SDL_FPoint { dx, dy });

	float dot = VectorUtils::DotProduct(facingDir, toPlayer);

	float centralCos = VectorUtils::DegreesToDotProduct(visionRange.coneAngles.centralConeAngle);
	float normalCos = VectorUtils::DegreesToDotProduct(visionRange.coneAngles.normalConeAngle);
	float peripheralCos = VectorUtils::DegreesToDotProduct(visionRange.coneAngles.peripheralConeAngle);

	/*
	* How this works:
	*	- calculate dot product between normalized facingDirection and toPlayer (vector that gives the direction towards the player)
	*	- calculate dot product between 
	*/

	if (dot >= centralCos && distance <= visionRange.tileRanges.centralTileRange * tileWidth)
	{
		return true;
	}

	if (dot >= normalCos && distance <= visionRange.tileRanges.centralTileRange * tileWidth)
	{
		return true;
	}

	if (dot >= peripheralCos && distance <= visionRange.tileRanges.peripheralTileRange * tileWidth)
	{
		return true;
	}

	return false;
}

void Guard::Init(Direction facingDirection)
{
	std::string&& tile = "Guard";
	if (IsSentinel(facingDirection))
	{
		facingDirection = Direction::Right;
	}

	const std::string& directionStr = DirectionToString(facingDirection);

	tile += StringMiscs::IsStringEnumSentinel(directionStr) ? "Right" : StringMiscs::CapitalizeFirst(directionStr);

	sprite = TileRegistry::GetTileIndexByName(tile);
	tileMover.SetCurrentDirection(facingDirection);

	GuardStateManager::ChangeToDefaultState(*this);

	if (guardCount == 0)
	{
		player = Player::GetInstance();
	}
	++guardCount;
}


