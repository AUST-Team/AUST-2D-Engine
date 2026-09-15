#include <SDL3/SDL.h>

#include "MapSaveLoadStrategy.h"

#include "Map.h"
#include "Guard.h"
#include "GuardManager.h"
#include "Trigger.h"
#include "TriggerFactory.h"
#include "TileRegistry.h"

MapSaveLoadStrategy::MapSaveLoadStrategy(const GameSaveData& saveData) :
	saveData(saveData) {}

void MapSaveLoadStrategy::LoadMap(Map& map)
{
    map.PlacePlayerAt(
        static_cast<float>(saveData.playerPosition.x),
        static_cast<float>(saveData.playerPosition.y)
    );

    for (const GuardSaveData& g : saveData.guards)
    {
        Guard* guard = GuardManager::GetInstance().GetGuard(g.guardId);
        if (guard)
        {
            guard->SetPosition(
                static_cast<float>(g.guardPosition.x), 
                static_cast<float>(g.guardPosition.y)
            );
        }
        else
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "MapSaveLoadStrategy.LoadMap: Guard with ID [%d] not found during save load.", g.guardId);
        }
    }

    for (const TriggerSaveData& s : saveData.triggers)
    {
        if (!s.isRuntimeFlag)
        {
            Trigger* t = map.GetTriggerById(s.id);
            if (!t)
            {
                continue;
            }

            if (!s.existsFlag)
            {
                map.RemoveTrigger(t);
                continue;
            }

            t->ApplyRuntimeState(s.runtimeState);
        }
        else
        {
            if (!s.existsFlag)
            {
                continue;
            }

            Trigger* t = TriggerFactory::CreateTriggerFromJSON(s.templateData, map.GetConditionRegistry());
            t->SetTriggerOrigin(TriggerOrigin::Runtime);
            t->ApplyRuntimeState(s.runtimeState);

            map.SetTrigger(t->GetPosition(), t);
        }
    }

    for (const TileSaveData& change : saveData.tiles)
    {
        if (change.tileIndex != ConstantConfiguration::invalidTileIndex)
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "MapSaveLoadStrategy.LoadMap: Saved tile change has invalid index.");
        }
        map.SetTile(change.position, change.tileIndex, change.mode);
    }

    for (const PresetSaveData& p : saveData.presets)
    {
        map.SetPreset(p.position, p.name, p.mode);
    }
}

void MapSaveLoadStrategy::SetSaveData(const GameSaveData& newData) { saveData = newData; }

GameSaveData MapSaveLoadStrategy::GetSaveData() const { return saveData; }
