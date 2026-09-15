#include <SDL3/SDL.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

#include "SaveManager.h"

#include "GameMap.h"
#include "Player.h"
#include "FlagManager.h"
#include "GameTime.h"
#include "GuardManager.h"
#include "Guard.h"
#include "MapSaveLoadStrategy.h"
#include "Camera.h"
#include "Trigger.h"
#include "MemoryTracker.h"
#include "FileLoader.h"
#include "IOMiscs.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

std::optional<GameSaveData> SaveManager::pendingSaveData = std::nullopt;
std::unordered_map<int, uint64_t> SaveManager::slotRevisions = {};

bool SaveManager::SaveGame(const GameSaveData& saveData, const fs::path& saveDir)
{
    const fs::path filePath = saveDir / IOMiscs::MakeCrossPlatformPath("save_" + std::to_string(saveData.id) + ".json");

    std::ofstream file(filePath);
    if (!file.is_open())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SaveManager.SaveGame: Failed to open file for writing: %s", filePath.string().c_str());
        return false;
    }

    const json j = saveData.ToJSON();
    try
    {
        file << j.dump(4);
    }
    catch(const std::exception& e)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SaveManager.SaveGame: Failed to write to file: %s", e.what());
        return false;
    }
    
    IncrementSlotRevision(saveData.id);
    return true;
}

bool SaveManager::LoadSave(int saveId, GameSaveData& outSave, const fs::path& saveDir)
{
    const fs::path filePath = saveDir / IOMiscs::MakeCrossPlatformPath("save_" + std::to_string(saveId) + ".json");
    const std::optional<json> jsonOptional = FileLoader::LoadJSON(filePath, JSONParseFlags::None);

    if (!jsonOptional)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SaveManager.LoadSave: Could not open file: %s", filePath.string().c_str());
        return false;
    }

    outSave = GameSaveData::FromJSON(*jsonOptional);

    // Loading doesn't modify the slot!
    return true;
}

bool SaveManager::DeleteSave(int saveId, const fs::path& saveDir)
{
    const fs::path filePath = saveDir / IOMiscs::MakeCrossPlatformPath("save_" + std::to_string(saveId) + ".json");

    std::error_code ec;
    bool removed = fs::remove(filePath, ec);

    if (ec || !removed)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
            "SaveManager.DeleteSave: Failed to delete save file %s: %s",
            filePath.string().c_str(),
            ec ? ec.message().c_str() : "File did not exist"
        );
        return false;
    }
    
    IncrementSlotRevision(saveId);
    return true;
}

GameSaveData SaveManager::CreateSave(int saveId)
{
    GameSaveData save{};

    const std::vector<Guard*>& guards = GuardManager::GetInstance().GetGuards();

    for (size_t i = 0; i < guards.size(); ++i)
    {
        if (!guards[i])
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SaveManager.CreateSave: Guard [%llu] is null. Skipping saving.", i);
            continue;
        }

        save.guards.emplace_back(GuardSaveData {
            .guardId = static_cast<int>(i),
            .guardPosition = SDL_Point {
                static_cast<int>(guards[i]->GetX()),
                static_cast<int>(guards[i]->GetY()),
            }
        });
    }

    const std::vector<Trigger*>& triggers = GameMap::GetInstance().GetAllTriggers();

    for (Trigger* t : triggers)
    {
        TriggerSaveData s;
        s.id = t->GetTriggerId();
        s.name = t->GetTriggerName();
        s.isRuntimeFlag = t->IsRuntime();
        s.existsFlag = !t->ShouldAutoDelete();
        s.runtimeState = t->SerializeRuntimeState();

        if (s.isRuntimeFlag)
        {
            s.templateData = t->GetTemplateJSON();
        }

        save.triggers.push_back(std::move(s));
    }

    save.tiles = GameMap::GetInstance().GetTileChanges();
    save.presets = GameMap::GetInstance().GetPresetChanges();

    save.id = saveId;
    save.mapName = GameMap::GetInstance().GetMapName();
    save.playerPosition = SDL_Point {
        static_cast<int>(Player::GetInstance()->GetX()),
        static_cast<int>(Player::GetInstance()->GetY()),
    };

    save.playerTimesCaught = Player::GetInstance()->GetTimesCaught();
    save.flags = FlagManager::GetInstance().ExportAsHex();
    save.timePlayedSeconds = GameTime::PeekPlayTimeSeconds();

    const Direction playerDir = Player::GetInstance()->GetLastDirection();
    
    save.playerDirection = DirectionToString(IsSentinel(playerDir) ? Direction::Right : playerDir);

    const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    const std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    std::tm local_tm{};
    localtime_s(&local_tm, &now_time);

    std::ostringstream oss;
    oss << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S");
    save.timestamp = oss.str();

    IncrementSlotRevision(saveId);
    return save;
}

void SaveManager::ForceApplySaveData(const GameSaveData& saveData)
{
    Player* player = Player::GetInstance();

    GameMap::GetInstance().LoadMapFromName(saveData.mapName, ENG_NEW(MapSaveLoadStrategy, saveData));
    GameTime::ContinueTracking(saveData.timePlayedSeconds);
    FlagManager::GetInstance().ImportFromHex(saveData.flags);

    player->SetTimesCaught(saveData.playerTimesCaught);
    player->ResetMovementState();
    player->SetPosition(
        static_cast<float>(saveData.playerPosition.x),
        static_cast<float>(saveData.playerPosition.y)
    );
    player->SetLastDirection(DirectionFromString(saveData.playerDirection));

    Camera::GetInstance()->SetFirstUpdate(true);
}

void SaveManager::ScheduleApplySaveData(const GameSaveData& data)
{
    if (pendingSaveData.has_value()) 
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "SaveManager.ScheduleApplySaveData: Overwriting pending save data!");
    }

    pendingSaveData = data;
}

void SaveManager::ApplyPendingSaveData()
{
    if (!pendingSaveData.has_value())
    {
        return;
    }

    ForceApplySaveData(pendingSaveData.value());
    pendingSaveData.reset();
}

bool SaveManager::HasPendingSave() { return pendingSaveData.has_value(); }

void SaveManager::ClearPendingSave() { pendingSaveData = std::nullopt; }

uint64_t SaveManager::GetSlotRevision(int slotId)
{
    auto it = slotRevisions.find(slotId);
    if (it != slotRevisions.end())
    {
        return it->second;
    }
    return 0; // 0 if slot has never been touched.
}

void SaveManager::IncrementSlotRevision(int slotId) { slotRevisions[slotId]++; } // Automatically inserts and increments if slotId isn't in map.

