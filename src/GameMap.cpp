#include <SDL3/SDL.h>
#include <stdexcept>
#include <filesystem>

#include "GameMap.h"

#include "Configuration.h"

#include "DialogueNarrationInteractable.h"
#include "TileRegistry.h"
#include "DialogueChoiceInteractable.h"
#include "Map.h"
#include "MapLoadStrategy.h"
#include "GuardManager.h"
#include "Player.h"
#include "AnimationSystem.h"
#include "Distances.h"
#include "VectorUtils.h"
#include "ConstantConfiguration.h"
#include "Audio.h"
#include "MemoryTracker.h"
#include "IOMiscs.h"
#include "Trigger.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

GameMap::~GameMap() { Cleanup(); }

void GameMap::Update(double deltaTime)
{
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.Update: Attempted to update on a null map.");
		return;
	}

	currentMap->Update(deltaTime, Player::GetInstance(), pendingUpdates);
	pendingUpdates = MapUpdateFlags::None;
	UpdateAnimationOverrides(deltaTime);
}

void GameMap::Cleanup()
{
	if (currentMap)
	{
		ENG_DELETE(currentMap);
		currentMap = nullptr;
	}
}

void GameMap::LoadMap(const fs::path& filePath, MapLoadStrategy* loadStrategy)
{
	if (currentMap)
	{
		ENG_DELETE(currentMap);
	}

	GuardManager::GetInstance().ClearGuards();

	currentMap = ENG_NEW(Map);
	currentMap->LoadMapFromFile(filePath);

	if (loadStrategy)
	{
		loadStrategy->LoadMap(*currentMap);

		ENG_DELETE(loadStrategy);
		loadStrategy = nullptr;
	}
	else
	{
		currentMap->PlacePlayerAtStart();
		Player::GetInstance()->SetLastDirection(currentMap->GetMapDefaultDirection());
	}

	RebuildCollisionCaches();
}

void GameMap::LoadMapFromName(const std::string& mapName, MapLoadStrategy* loadStrategy)
{
	const fs::path path = Configuration::Get().paths.mapDirectory / IOMiscs::MakeCrossPlatformPath(mapName + ".json");
	LoadMap(path, loadStrategy);
}

std::string GameMap::GetMapName() const
{
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.GetMapName: Attempted to get map id on a null map.");
		return "";
	}

	return currentMap->GetName();
}

void GameMap::SetTile(int cellX, int cellY, int tileIndex, TilePlacementMode placementMode)
{
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.SetTile: Attempted to set a tile on a null map.");
		return;
	}

	if (!IsInMapBounds(cellX, cellY))
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.SetTile: Attempted to set a tile outside of map bounds [x = %d, y = %d].", cellX, cellY);
		return;
	}

	const TileSaveData change {
		.position = SDL_Point { cellX, cellY },
		.tileIndex = tileIndex,
		.mode = placementMode
	};
	tileChanges.push_back(change);
	currentMap->SetTile(cellX, cellY, tileIndex, placementMode);
	RebuildCollisionCaches();
}

void GameMap::SetTile(const SDL_Point& cellPosition, int tileIndex, TilePlacementMode placementMode) { SetTile(cellPosition.x, cellPosition.y, tileIndex, placementMode); }

void GameMap::SetTile(int cellX, int cellY, const std::string& tileName, TilePlacementMode placementMode)
{
	const int tileIndex = TileRegistry::GetTileIndexByName(tileName);
	if (tileIndex == ConstantConfiguration::invalidTileIndex)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.SetTile: Invalid tile index for name %s.", tileName.c_str());
		return;
	}

	SetTile(cellX, cellY, tileIndex, placementMode);
}

void GameMap::SetTile(const SDL_Point& cellPosition, const std::string& tileName, TilePlacementMode placementMode) { SetTile(cellPosition.x, cellPosition.y, tileName, placementMode); }

void GameMap::SetTile(const nlohmann::json& tileJSON)
{
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.SetTile: Attempted to set a tile on a null map.");
		return;
	}

	currentMap->SetTile(tileJSON, &tileChanges);
	RebuildCollisionCaches();
}

int GameMap::GetTile(int cellX, int cellY) const
{
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.GetTile: Attempted to get a tile on a null map.");
		return ConstantConfiguration::invalidTileIndex;
	}

	if (!IsInMapBounds(cellX, cellY))
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.GetTile: Attempted to get a tile out of bounds [x = %d, y = %d].", cellX, cellY);
		return ConstantConfiguration::invalidTileIndex;
	}

	return currentMap->GetTile(cellX, cellY);
}

int GameMap::GetTile(const SDL_Point& cellPosition) const { return GetTile(cellPosition.x, cellPosition.y); }

const std::vector<int>& GameMap::GetTiles(int cellX, int cellY) const
{
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.GetTiles: Attempted to get tiles on a null map.");
		static std::vector<int> emptyVector;
		return emptyVector;
	}

	if (!IsInMapBounds(cellX, cellY))
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.GetTiles: Attempted to get tiles out of bounds [x = %d, y = %d].", cellX, cellY);
		static std::vector<int> emptyVector;
		return emptyVector;
	}

	return currentMap->GetTiles(cellX, cellY);
}

const std::vector<int>& GameMap::GetTiles(const SDL_Point& cellPosition) const { return GetTiles(cellPosition.x, cellPosition.y); }

void GameMap::SetPreset(int presetX, int presetY, const std::string& presetName, TilePlacementMode placementMode)
{
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.SetPreset: Attempted to set a preset on a null map.");
		return;
	}

	if (!IsInMapBounds(presetX, presetY))
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.SetPreset: Attempted to set a preset out of bounds [x = %d, y = %d].", presetX, presetY);
		return;
	}

	const PresetSaveData preset {
		.name = presetName,
		.position = SDL_Point { presetX, presetY },
		.mode = placementMode
	};
	presetChanges.push_back(preset);

	currentMap->SetPreset(presetX, presetY, presetName, placementMode);
	RebuildCollisionCaches();
}

void GameMap::SetPreset(const SDL_Point& presetPosition, const std::string& presetName, TilePlacementMode placementMode) { SetPreset(presetPosition.x, presetPosition.y, presetName, placementMode); }

void GameMap::SetPreset(const nlohmann::json& presetJSON)
{
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.SetPreset: Attempted to set a preset on a null map.");
		return;
	}

	currentMap->SetPreset(presetJSON, &presetChanges);
	RebuildCollisionCaches();
}

int GameMap::GetMapWidth() const
{ 
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.GetMapWidth: Attempted to get map width on a null map.");
		return 0;
	}

	return currentMap->GetWidth();
}

int GameMap::GetMapHeight() const 
{ 
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.GetMapHeight: Attempted to get map height on a null map.");
		return 0;
	}

	return currentMap->GetHeight();
}

int GameMap::GetMapWidthPixels() const 
{ 
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.GetMapWidthPixels: Attempted to get map width on a null map.");
		return 0;
	}

	return currentMap->GetWidth() * Configuration::Get().tiles.width;
}

int GameMap::GetMapHeightPixels() const 
{
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.GetMapHeightPixels: Attempted to get map height on a null map.");
		return 0;
	}

	return currentMap->GetHeight() * Configuration::Get().tiles.height;
}

void GameMap::SetTrigger(int triggerX, int triggerY, Trigger* trigger)
{
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.SetTrigger: Attempted to add trigger on a null map.");
		return;
	}

	if (!IsInMapBounds(triggerX, triggerY))
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.SetTrigger: Attempted to add trigger out of bounds [x = %d, y = %d].", triggerX, triggerY);
		return;
	}

	currentMap->SetTrigger(triggerX, triggerY, trigger);
}

void GameMap::SetTrigger(const SDL_Point& triggerPosition, Trigger* trigger) { SetTrigger(triggerPosition.x, triggerPosition.y, trigger); }

void GameMap::SetTrigger(const nlohmann::json& triggerJSON, TriggerOrigin origin)
{
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.SetTrigger: Attempted to add trigger on a null map.");
		return;
	}

	Trigger* triggerPtr = currentMap->SetTrigger(triggerJSON);

	if (triggerPtr && origin == TriggerOrigin::Runtime)
	{
		triggerPtr->SetTriggerOrigin(TriggerOrigin::Runtime);
		triggerPtr->SetTemplateJSON(triggerJSON);
	}
}

std::vector<Trigger *>& GameMap::GetTriggers(int cellX, int cellY)
{
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.GetTriggers: Attempted to get trigger on a null map.");
		static std::vector<Trigger*> emptyVector;
		return emptyVector;
	}

	if (!IsInMapBounds(cellX, cellY))
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.GetTriggers: Attempted to get trigger out of bounds [x = %d, y = %d].", cellX, cellY);
		static std::vector<Trigger*> emptyVector;
		return emptyVector;
	}

	return currentMap->GetTriggers(cellX, cellY);
}

std::vector<Trigger *>& GameMap::GetTriggers(const SDL_Point& cellPosition) { return GetTriggers(cellPosition.x, cellPosition.y); }

std::vector<Trigger*> GameMap::GetAllTriggers()
{
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.GetAllTriggers: Attempted to get triggers on a null map.");
		static std::vector<Trigger*> emptyVector;
		return emptyVector;
	}

	return currentMap->GetAllTriggers();
}

ConditionRegistry& GameMap::GetConditionRegistry()
{
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.GetConditionRegistry: Attempted to get condition registry on a null map.");
		static ConditionRegistry emptyRegistry;
		return emptyRegistry;
	}

	return currentMap->GetConditionRegistry();
}

bool GameMap::IsInMapBounds(int x, int y) const
{
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.IsInMapBounds: Attempted to check a point on a null map.");
		return false;
	}

	return currentMap->IsInMapBounds(x, y);
}

bool GameMap::IsInMapBounds(const SDL_Point& point) const { return IsInMapBounds(point.x, point.y); }

bool GameMap::IsTileSolid(int cellX, int cellY) const
{
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.IsTileSolid: Attempted to check a tile on a null map.");
		return false;
	}

	if (!IsInMapBounds(cellX, cellY))
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.IsTileSolid: Attempted to check a tile out of bounds at [x = %d, y = %d].", cellX, cellY);
		return false;
	}

	return collisionGrid[Index(cellX, cellY)].solidFlag;
}

bool GameMap::IsTileSolid(const SDL_Point& cellPosition) const { return IsTileSolid(cellPosition.x, cellPosition.y); }

bool GameMap::IsTileBlockingVision(int cellX, int cellY) const
{
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.IsTileBlockingVision: Attempted to check a tile on a null map.");
		return false;
	}

	if (!IsInMapBounds(cellX, cellY))
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.IsTileBlockingVision: Attempted to check a tile out of bounds at [x = %d, y = %d].", cellX, cellY);
		return false;
	}

	return collisionGrid[Index(cellX, cellY)].blocksVisionFlag;
}

bool GameMap::IsTileBlockingVision(const SDL_Point& cellPosition) const { return IsTileBlockingVision(cellPosition.x, cellPosition.y); }

const std::vector<TileAnchorInstance>& GameMap::GetAnchorsCoveringTile(int cellX, int cellY) const
{
	return GetAnchorsCoveringTile(SDL_Point { cellX, cellY });
}

const std::vector<TileAnchorInstance>& GameMap::GetAnchorsCoveringTile(const SDL_Point& cellPosition) const 
{ 
	if (!IsInMapBounds(cellPosition.x, cellPosition.y))
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.GetAnchorsCoveringTile: Attempted to check a tile out of bounds at [%d, %d].", cellPosition.x, cellPosition.y);
		static const std::vector<TileAnchorInstance> empty;
		return empty;
	}

	auto it = anchorCoverage.find(cellPosition);
	if (it != anchorCoverage.cend())
	{
		return it->second;
	}

	static const std::vector<TileAnchorInstance> empty;
	return empty;
}

void GameMap::SetAnimationOverride(int cellX, int cellY, TileAnimationType animationType, OverrideType overrideType, bool oneShot, int tileIndexFilter)
{
	SetAnimationOverride(SDL_Point { cellX, cellY }, animationType, overrideType, oneShot, tileIndexFilter);
}

// I forgot which refactor number this is, but there's a lot.
void GameMap::SetAnimationOverride(const SDL_Point& cellPosition, TileAnimationType animationType, OverrideType overrideType, bool oneShot, int tileIndexFilter)
{ 
	if (!IsInMapBounds(cellPosition.x, cellPosition.y))
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.SetAnimationOverride: Attempted to set override on a tile out of bounds at [%d, %d].", cellPosition.x, cellPosition.y);
		return;
	}

	if (!animationSystem)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.SetAnimationOverride: Animation system is null.");
		return;
	}

	const float globalStartTime = animationSystem->GetGlobalAnimationTime();
	const bool fullOverride = (overrideType == OverrideType::Full);
	const size_t animIndex = TileReactionIndexFromType(animationType);

	// The tiles at the cell.
	if (fullOverride)
	{
		// Full overrides ignore relevance.
		const std::vector<int>& tiles = currentMap->GetTiles(cellPosition.x, cellPosition.y);

		// For all tiles on this cell.
		for (int tileIndex : tiles)
		{
			// Filter.
			if (tileIndexFilter != ConstantConfiguration::invalidTileIndex && tileIndex != tileIndexFilter)
			{
				continue;
			}

			// Get tile instance.
			const TileInstance* tile = TileRegistry::GetTileTypeByIndex(tileIndex);
			if (!tile || !HasFlag(tile->animationMask, animationType))
			{
				continue;
			}

			ApplyAnimationOverrideAtAnchor(cellPosition, tileIndex, animationType, overrideType, oneShot, globalStartTime);
		}
	}
	else
	{
		// Semi overrides take into account relevance.
		// Get the map of [Point, tileIndexes]
		const std::unordered_map<SDL_Point, std::unordered_set<int>>& byAnim = animationRelevance[animIndex];
		// Find this cell's position.
		auto cellIt = byAnim.find(cellPosition);
		if (cellIt != byAnim.cend())
		{
			// For every tile in the cell.
			for (int tileIndex : cellIt->second)
			{
				// Filter
				if (tileIndexFilter != ConstantConfiguration::invalidTileIndex && tileIndex != tileIndexFilter)
				{
					continue;
				}

				ApplyAnimationOverrideAtAnchor(cellPosition, tileIndex, animationType, overrideType, oneShot, globalStartTime);
			}
		}
	}

	// Handle anchors covering this cell.
	auto anchorIt = anchorCoverage.find(cellPosition);
	if (anchorIt == anchorCoverage.end())
	{
		return;
	}

	// For every anchor covering the cell.
	for (const TileAnchorInstance& ref : anchorIt->second)
	{
		// Filter.
		if (tileIndexFilter != ConstantConfiguration::invalidTileIndex && ref.tileIndex != tileIndexFilter)
		{
			continue;
		}

		// If full override.
		if (fullOverride)
		{
			// Ignore relevance, get tile.
			const TileInstance* tile = TileRegistry::GetTileTypeByIndex(ref.tileIndex);

			// Check if it can support animation.
			if (!tile || !HasFlag(tile->animationMask, animationType))
			{
				continue;
			}
		}
		else
		{
			// Semi overrides check for relevance.
			const std::unordered_map<SDL_Point, std::unordered_set<int>>& byAnim = animationRelevance[animIndex];
			auto anchorCellIt = byAnim.find(ref.anchorPosition);

			if (anchorCellIt == byAnim.cend())
			{
				continue;
			}

			if (anchorCellIt->second.find(ref.tileIndex) == anchorCellIt->second.end())
			{
				continue;
			}
		}

		ApplyAnimationOverrideAtAnchor(ref.anchorPosition, ref.tileIndex, animationType, overrideType, oneShot, globalStartTime);
	}
	// All this for O(1) look-up.
}

void GameMap::RemoveAnimationOverride(int cellX, int cellY, OverrideType overrideType, TileAnimationType animationType, int tileIndexFilter)
{
	RemoveAnimationOverride(SDL_Point { cellX, cellY }, overrideType, animationType, tileIndexFilter);
}

void GameMap::RemoveAnimationOverride(const SDL_Point& cellPosition, OverrideType overrideType, TileAnimationType animationType, int tileIndexFilter)
{
	if (!IsInMapBounds(cellPosition.x, cellPosition.y))
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.RemoveAnimationOverride: Attempted to remove override on a tile out of bounds at [%d, %d].", cellPosition.x, cellPosition.y);
		return;
	}

	// I swear generic programming makes sense.
	// Override maps are maps of [Point, map<tileIndex, override>].
	auto removeFromMap = [&](auto& overrideMap) {
		auto mapIt = overrideMap.find(cellPosition);
		if (mapIt == overrideMap.end())
		{
			return;
		}

		auto& inner = mapIt->second;

		for (auto it = inner.begin(); it != inner.end(); )
		{
			const int tileIndex = it->first;
			auto& ov = it->second;

			if (tileIndexFilter != ConstantConfiguration::invalidTileIndex && tileIndex != tileIndexFilter)
			{
				++it;
				continue;
			}

			if (ov.type != animationType)
			{
				++it;
				continue;
			}

			it = inner.erase(it);
		}

		if (inner.empty())
		{
			overrideMap.erase(mapIt);
		}
	};

	if (overrideType == OverrideType::Semi)
	{
		removeFromMap(tileAnimationOverrides.semiOverrides);
		hasAnimationSemiOverrides = !tileAnimationOverrides.semiOverrides.empty();
	}
	else
	{
		removeFromMap(tileAnimationOverrides.fullOverrides);
		hasAnimationFullOverrides = !tileAnimationOverrides.fullOverrides.empty();
	}
}

void GameMap::RemoveAllAnimationOverrides(int cellX, int cellY)
{
	RemoveAllAnimationOverrides(SDL_Point { cellX, cellY });
}

void GameMap::RemoveAllAnimationOverrides(const SDL_Point& cellPosition)
{
	tileAnimationOverrides.semiOverrides.erase(cellPosition);
	tileAnimationOverrides.fullOverrides.erase(cellPosition);
}

AnimationOverrideView GameMap::GetAnimationOverride(int anchorX, int anchorY, int tileIndex)
{ 
	return GetAnimationOverride(SDL_Point { anchorX, anchorY }, tileIndex);
}

AnimationOverrideView GameMap::GetAnimationOverride(const SDL_Point& anchorPosition, int tileIndex)
{
	AnimationOverrideView result{};

	if (!IsInMapBounds(anchorPosition.x, anchorPosition.y))
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.GetAnimationOverride: Attempted to get override of tile out of bounds at [%d, %d].", anchorPosition.x, anchorPosition.y);
		return result;
	}

	if (auto it = tileAnimationOverrides.fullOverrides.find(anchorPosition);
		it != tileAnimationOverrides.fullOverrides.end())
	{
		std::unordered_map<int, FullAnimationOverride>& inner = it->second;
		auto oIt = inner.find(tileIndex);

		if (oIt != inner.end())
		{
			result.full = &oIt->second;
		}
	}

	if (auto it = tileAnimationOverrides.semiOverrides.find(anchorPosition);
		it != tileAnimationOverrides.semiOverrides.end())
	{
		std::unordered_map<int, SemiAnimationOverride>& inner = it->second;
		auto oIt = inner.find(tileIndex);

		if (oIt != inner.end())
		{
			result.semi = &oIt->second;
		}
	}

	return result;
}

void GameMap::ApplyLookAnimation(int playerX, int playerY, Direction playerDirection, OverrideType overrideType, bool oneShot, int range, int tileIndexFilter)
{
	ApplyLookAnimation(SDL_Point { playerX, playerY }, playerDirection, overrideType, oneShot, range, tileIndexFilter);
}

void GameMap::ApplyLookAnimation(const SDL_Point& playerPosition, Direction playerDirection, OverrideType overrideType, bool oneShot, int range, int tileIndexFilter)
{
	if (!IsInMapBounds(playerPosition.x, playerPosition.y))
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.ApplyLookAnimation: Attempted to set override with player position out of bounds at [%d, %d].", playerPosition.x, playerPosition.y);
		return;
	}

	const TileAnimationType types[2] = { TileAnimationType::LookAt, TileAnimationType::LookAway };

	const SDL_FPoint dirVector = GetDirectionOffsetFloat(playerDirection);

	for (const TileAnimationType animType : types)
	{
		const std::unordered_map<SDL_Point, std::unordered_set<int>>& typeIt = animationRelevance[TileReactionIndexFromType(animType)];

		for (const auto& [cellPos, tileIndices] : typeIt)
		{
			const int distanceSq = Distances::GetManhattanDistanceTo(cellPos, playerPosition);
			if (distanceSq > range)
			{
				continue;
			}

			// Decide LookAt vs LookAway based on player direction.
			const SDL_FPoint tileCenter {
				cellPos.x + 0.5f, // cellPos.x is the upper left corner.
				cellPos.y + 0.5f
			};

			const SDL_FPoint toTile {
				tileCenter.x - playerPosition.x,
				tileCenter.y - playerPosition.y
			};

			const float dot = VectorUtils::DotProduct(toTile, dirVector);
			const TileAnimationType chosenAnim = (dot >= 0) ? TileAnimationType::LookAt : TileAnimationType::LookAway;

			if (chosenAnim != animType)
			{
				continue;
			}

			SetAnimationOverride(cellPos, animType, overrideType, oneShot, tileIndexFilter);
		}
	}
}

void GameMap::PlayAudioAt(int cellX, int cellY, TileSoundType soundType, int tileIndexFilter)
{
	PlayAudioAt(SDL_Point { cellX, cellY }, soundType, tileIndexFilter);
}

void GameMap::PlayAudioAt(const SDL_Point& cellPosition, TileSoundType soundType, int tileIndexFilter)
{
	if (!IsInMapBounds(cellPosition.x, cellPosition.y))
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.PlayAudioAt: Attempted to play audio on a tile out of bounds at [%d, %d].", cellPosition.x, cellPosition.y);
		return;
	}

	const size_t soundIndex = TileReactionIndexFromType(soundType);

	// Get the map of [Point, tileIndexes]
	const std::unordered_map<SDL_Point, std::unordered_set<int>>& bySound = soundRelevance[soundIndex];
	// Find this cell's position.
	auto cellIt = bySound.find(cellPosition);
	if (cellIt != bySound.cend())
	{
		// For every tile in the cell.
		for (const int tileIndex : cellIt->second)
		{
			// Filter
			if (tileIndexFilter != ConstantConfiguration::invalidTileIndex && tileIndex != tileIndexFilter)
			{
				continue;
			}

			const TileInstance* tile = TileRegistry::GetTileTypeByIndex(tileIndex);

			// Check if it can support audio.
			if (!tile || !HasFlag(tile->soundMask, soundType))
			{
				continue;
			}

			// Play audio.
			const TileAudio& tileAudio = tile->audioTable[soundIndex];
			tileAudio.PlayTileAudio();
		}
	}

	// Handle anchors covering this cell.
	auto anchorIt = anchorCoverage.find(cellPosition);
	if (anchorIt == anchorCoverage.end())
	{
		return;
	}

	// For every anchor covering the cell.
	for (const TileAnchorInstance& ref : anchorIt->second)
	{
		// Semi overrides check for relevance.
		auto anchorCellIt = bySound.find(ref.anchorPosition);

		if (anchorCellIt == bySound.end())
		{
			continue;
		}

		// Check if the tile index exists.
		if (anchorCellIt->second.find(ref.tileIndex) == anchorCellIt->second.end())
		{
			continue;
		}

		// Filter.
		if (tileIndexFilter != ConstantConfiguration::invalidTileIndex && ref.tileIndex != tileIndexFilter)
		{
			continue;
		}

		// Get tile.
		const TileInstance* tile = TileRegistry::GetTileTypeByIndex(ref.tileIndex);

		// Check if it can support audio.
		if (!tile || !HasFlag(tile->soundMask, soundType))
		{
			continue;
		}

		// Check if it can support audio.
		const TileAudio& tileAudio = tile->audioTable[soundIndex];
		tileAudio.PlayTileAudio();
	}
}

void GameMap::ApplyLookSound(int playerX, int playerY, Direction playerDirection, int range, int tileIndexFilter)
{
	ApplyLookSound(SDL_Point { playerX, playerY }, playerDirection, range, tileIndexFilter);
}

void GameMap::ApplyLookSound(const SDL_Point& playerPosition, Direction playerDirection, int range, int tileIndexFilter)
{
	if (!IsInMapBounds(playerPosition.x, playerPosition.y))
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.ApplyLookSound: Attempted to play sound with player position out of bounds at [%d, %d].", playerPosition.x, playerPosition.y);
		return;
	}

	const TileSoundType types[2] = { TileSoundType::LookAt, TileSoundType::LookAway };

	const SDL_FPoint dirVector = GetDirectionOffsetFloat(playerDirection);

	for (const TileSoundType soundType : types)
	{
		const std::unordered_map<SDL_Point, std::unordered_set<int>>& typeIt = soundRelevance[TileReactionIndexFromType(soundType)];

		for (const auto& [cellPos, tileIndices] : typeIt)
		{
			const int distanceSq = Distances::GetManhattanDistanceTo(cellPos, playerPosition);
			if (distanceSq > static_cast<int>(range))
			{
				continue;
			}

			// Decide LookAt vs LookAway based on player direction.
			const SDL_FPoint tileCenter {
				cellPos.x + 0.5f, // cellPos.x is the upper left corner.
				cellPos.y + 0.5f
			};

			const SDL_FPoint toTile {
				tileCenter.x - playerPosition.x,
				tileCenter.y - playerPosition.y
			};

			const float dot = VectorUtils::DotProduct(toTile, dirVector);
			const TileSoundType chosenSound = (dot >= 0) ? TileSoundType::LookAt : TileSoundType::LookAway;

			if (chosenSound != soundType)
			{
				continue;
			}

			PlayAudioAt(cellPos, soundType, tileIndexFilter);
		}
	}
}

std::vector<TileSaveData> GameMap::GetTileChanges() { return tileChanges; }

std::vector<PresetSaveData> GameMap::GetPresetChanges() { return presetChanges; }

void GameMap::AddPendingUpdateFlags(MapUpdateFlags flags) { pendingUpdates |= flags; }

void GameMap::RequestInteract() { pendingUpdates |= MapUpdateFlags::Interact; }

void GameMap::NotifyPlayerMoved() { pendingUpdates |= MapUpdateFlags::PlayerMoved; }

void GameMap::NotifyPlayerChangedDirection() { pendingUpdates |= MapUpdateFlags::PlayerDirectionChange; }

void GameMap::SetAnimationSystem(AnimationSystem* animationSystemPtr) { animationSystem = animationSystemPtr; }

DialogueData GameMap::GetDialogueData() const
{
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.GetDialogueData: Attempted to get dialogue data on a null map.");
		return DialogueData{};
	}

	return currentMap->GetDefaultDialogue();
}

void GameMap::RebuildCollisionCaches()
{
	if (!currentMap)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.RebuildCollisionCaches: Attempted to rebuild caches on a null map.");
		return;
	}

	const int width = currentMap->GetWidth();
	const int height = currentMap->GetHeight();
	collisionGrid.assign(static_cast<size_t>(width) * height, CollisionCell());
	anchorCoverage.clear();
	animationRelevance.fill({});
	soundRelevance.fill({});

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			const std::vector<int>& cell = currentMap->GetTiles(x, y);

			for (const int tileIndex : cell)
			{
				const TileInstance* tile = TileRegistry::GetTileTypeByIndex(tileIndex);

				if (!tile)
				{
					continue;
				}

				const SDL_Point anchor { x, y };

				for (const TileCollisionPart& part : tile->collisionParts)
				{
					const int cx = x + part.offset.x;
					const int cy = y + part.offset.y;

					if (!IsInMapBounds(cx, cy))
					{
						continue;
					}

					const int idx = cy * width + cx;

					collisionGrid[idx].solidFlag |= part.solidFlag;
					collisionGrid[idx].blocksVisionFlag |= part.blocksVisionFlag;
				}

				if(tile->hasNonDefaultAnimations)
				{
					// For each part.
					for (const TileRenderPart& part : tile->renderParts)
					{
						// Get the non default 
						const TileAnimationType nonDefault = part.allowedAnimationsMask & ~part.defaultAnimation;

						// Check if there are any non default animation.
						if (IsNone(nonDefault))
						{
							continue;
						}

						// For each non-default animation.
						for (TileReactionTypeUnderlying bit = 1; bit != 0; bit <<= 1)
						{
							const TileAnimationType animBit = static_cast<TileAnimationType>(bit);
							if (!HasFlag(nonDefault, animBit))
							{
								continue;
							}

							// Push tileIndex in the structure of [animationType, [point, set<index>]].
							animationRelevance[TileReactionIndexFromType(animBit)][anchor].insert(tileIndex);
						}
					}
				}

				if (tile->hasNonDefaultSounds)
				{
					// For each part.
					for (const TileMetadataPart& part : tile->metadataParts)
					{
						// Get the non default 
						const TileSoundType nonDefault = part.allowedSoundsMask & ~part.defaultSound;

						// Check if there are any non default sounds.
						if (IsNone(nonDefault))
						{
							continue;
						}

						// For each non-default animation.
						for (TileReactionTypeUnderlying bit = 1; bit != 0; bit <<= 1)
						{
							const TileSoundType soundBit = static_cast<TileSoundType>(bit);
							if (!HasFlag(nonDefault, soundBit))
							{
								continue;
							}

							// Push tileIndex in the structure of [animationType, [point, set<index>]].
							soundRelevance[TileReactionIndexFromType(soundBit)][anchor].insert(tileIndex);
						}
					}
				}

				if (tile->footprint.h == 1 && tile->footprint.w == 1)
				{
					continue;
				}

				for (int dy = 0; dy < tile->footprint.h; ++dy)
				{
					for (int dx = 0; dx < tile->footprint.w; ++dx)
					{
						const int cx = x + dx;
						const int cy = y - dy; // Footprint is positive, but rows go other way.

						// Don't put the anchor's position.
						if (cx == x && cy == y)
						{
							continue;
						}

						if (!IsInMapBounds(cx, cy))
						{
							continue;
						}

						// Covered is at [cx, cy], while anchor is at [x, y]
						const SDL_Point covered { cx, cy };

						std::vector<TileAnchorInstance>& list = anchorCoverage[covered];
						TileAnchorInstance anchorInstance(anchor, tileIndex);
						if (std::find(list.begin(), list.end(), anchorInstance) == list.end())
						{
							list.push_back(std::move(anchorInstance));
						}
					}
				}
			}
		}
	}
}

void GameMap::UpdateAnimationOverrides(double deltaTime)
{
	if (hasAnimationFullOverrides)
	{
		for (auto mapIt = tileAnimationOverrides.fullOverrides.begin();
			mapIt != tileAnimationOverrides.fullOverrides.end(); )
		{
			std::unordered_map<int, FullAnimationOverride>& inner = mapIt->second;

			for (auto it = inner.begin(); it != inner.end(); )
			{
				FullAnimationOverride& o = it->second;

				o.state.Advance(deltaTime, o.clip);

				if (o.oneShotFlag && o.state.IsFinished(o.clip))
				{
					it = inner.erase(it);
				}
				else
				{
					++it;
				}
			}

			if (inner.empty())
			{
				mapIt = tileAnimationOverrides.fullOverrides.erase(mapIt);
			}
			else
			{
				++mapIt;
			}
		}

		hasAnimationFullOverrides = !tileAnimationOverrides.fullOverrides.empty();
	}

	if (hasAnimationSemiOverrides && animationSystem)
	{
		for (auto mapIt = tileAnimationOverrides.semiOverrides.begin();
			mapIt != tileAnimationOverrides.semiOverrides.end(); )
		{
			std::unordered_map<int, SemiAnimationOverride>& inner = mapIt->second;

			for (auto it = inner.begin(); it != inner.end(); )
			{
				const SemiAnimationOverride& o = it->second;

				if (o.oneShotFlag)
				{
					TileInstance* tile = TileRegistry::GetTileTypeByIndex(o.tileIndex);
					if (!tile)
					{
						it = inner.erase(it);
						continue;
					}

					if (animationSystem->IsAnimationFinished(tile, o.type, o.startTime))
					{
						it = inner.erase(it);
						continue;
					}
				}

				++it;
			}

			if (inner.empty())
			{
				mapIt = tileAnimationOverrides.semiOverrides.erase(mapIt);
			}
			else
			{
				++mapIt;
			}
		}

		hasAnimationSemiOverrides = !tileAnimationOverrides.semiOverrides.empty();
	}
}

void GameMap::ApplyAnimationOverrideAtAnchor(int anchorX, int anchorY, int tileIndex, TileAnimationType animationType, OverrideType overrideType, bool oneShot, float startTime)
{
	ApplyAnimationOverrideAtAnchor(SDL_Point { anchorX, anchorY }, tileIndex, animationType, overrideType, oneShot, startTime);
}

void GameMap::ApplyAnimationOverrideAtAnchor(const SDL_Point& anchorPosition, int tileIndex, TileAnimationType animationType, OverrideType overrideType, bool oneShot, float startTime)
{
	switch (overrideType)
	{
		case OverrideType::Semi:
		{
			std::unordered_map<int, SemiAnimationOverride>& inner = tileAnimationOverrides.semiOverrides[anchorPosition];
			inner[tileIndex] = SemiAnimationOverride(tileIndex, animationType, oneShot, startTime);
			hasAnimationSemiOverrides = true;
			break;
		}

		case OverrideType::Full:
		{
			std::unordered_map<int, FullAnimationOverride>& inner = tileAnimationOverrides.fullOverrides[anchorPosition];
			inner[tileIndex] = FullAnimationOverride(tileIndex, animationType, oneShot);
			hasAnimationFullOverrides = true;
			break;
		}

		default:
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.ApplyAnimationOverrideAtAnchor: Invalid override type.");
			break;
		}
	}
}

void GameMap::RemoveAnimationOverrideAtAnchor(int anchorX, int anchorY, int tileIndex, OverrideType overrideType)
{
	RemoveAnimationOverrideAtAnchor(SDL_Point { anchorX, anchorY }, tileIndex, overrideType);
}

void GameMap::RemoveAnimationOverrideAtAnchor(const SDL_Point& anchorCell, int tileIndex, OverrideType overrideType)
{
	switch (overrideType)
	{
		case OverrideType::Semi:
		{
			auto it = tileAnimationOverrides.semiOverrides.find(anchorCell);
			if (it != tileAnimationOverrides.semiOverrides.end())
			{
				it->second.erase(tileIndex);
				if (it->second.empty())
				{
					tileAnimationOverrides.semiOverrides.erase(it);
				}
			}
			hasAnimationSemiOverrides = !tileAnimationOverrides.semiOverrides.empty();
			break;
		}

		case OverrideType::Full:
		{
			auto it = tileAnimationOverrides.fullOverrides.find(anchorCell);
			if (it != tileAnimationOverrides.fullOverrides.end())
			{
				it->second.erase(tileIndex);
				if (it->second.empty())
				{
					tileAnimationOverrides.fullOverrides.erase(it);
				}
			}
			hasAnimationFullOverrides = !tileAnimationOverrides.fullOverrides.empty();
			break;
		}

		default:
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GameMap.RemoveAnimationOverrideAtAnchor: Invalid override type.");
			break;
		}
	}
}

size_t GameMap::Index(int x, int y) const { return static_cast<size_t>(y) * currentMap->GetWidth() + x; }
