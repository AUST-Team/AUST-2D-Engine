#pragma once

#ifndef SAVEMANAGER_H_
#define SAVEMANAGER_H_

#include <unordered_map>
#include <filesystem>

#include "GameSaveData.h"
#include "Configuration.h"

class SaveManager
{
private:
	static std::optional<GameSaveData> pendingSaveData;	/// Save data.
	static std::unordered_map<int, uint64_t> slotRevisions;	/// Slot revision 'stamps' per slot [slotID; revisionNumber].

	// No instancing due to static class.
	SaveManager() = delete;
	~SaveManager() = delete;

	// No copying or moving due to static class.
	SaveManager(const SaveManager&) = delete;
	SaveManager& operator=(const SaveManager&) = delete;
	SaveManager(SaveManager&&) = delete;
	SaveManager& operator=(SaveManager&&) = delete;

public:

	/**
	* @brief Saves the game.
	*
	* @param saveData Data to be saved.
	* @param saveDir Directory of the save file.
	*
	* @return 'true' if saving was successful, 'false' if otherwise.
	*/
	static bool SaveGame(const GameSaveData& saveData, const std::filesystem::path& saveDir = Configuration::Get().paths.saveDirectory);

	/**
	* @brief Loads a save file.
	*
	* @param saveId ID of the save file.
	* @param [out] outSave The save file data.
	* @param saveDir Directory of the save file.
	*
	* @return 'true' if loading was successful, 'false' if otherwise.
	*/
	static bool LoadSave(int saveId, GameSaveData& outSave, const std::filesystem::path& saveDir = Configuration::Get().paths.saveDirectory);

	/**
	* @brief Deletes a save file.
	*
	* @param saveId ID of the save file.
	* @param saveDir Directory of the save file.
	*
	* @return 'true' if deleting was successful, 'false' if otherwise.
	*/
	static bool DeleteSave(int saveId, const std::filesystem::path& saveDir = Configuration::Get().paths.saveDirectory);

	/**
	* @brief Creates the save data.
	*
	* @param saveId ID of the save file.
	*
	* @return The save data structure created.
	*/
	static GameSaveData CreateSave(int saveID);

	/**
	* @brief Applies the save data.
	*
	* This should only be used when you know what you're doing.
	*
	* @param saveData The save data to be used.
	*/
	static void ForceApplySaveData(const GameSaveData& saveData);

	/**
	* @brief Schedules applying save data.
	*
	* @param saveData The save data to be used.
	*/
	static void ScheduleApplySaveData(const GameSaveData& data);

	/**
	* @brief Applies pending save data.
	*/
	static void ApplyPendingSaveData();

	/**
	* @brief Checks if the save manager has save data pending to be applied.
	*
	* @return 'true' if there's pending save data to be applied, 'false' if otherwise.
	*/
	static bool HasPendingSave();

	/**
	* @brief Clears the pending save data.
	*/
	static void ClearPendingSave();

	/**
	* @brief Returns the save slot revision of a certain slot.
	* 
	* @param slotId The ID of the slot.
	*
	* @return The revision number of said slot, or '0' if the slot has not been created, modified or manipulated in any way.
	*/
	static uint64_t GetSlotRevision(int slotId);

	/**
	* @brief Increments a slot's revision number.
	* 
	* @param slotId The ID of the save / load slot.
	*/
	static void IncrementSlotRevision(int slotId);

	static bool SaveGame(const GameSaveData& saveData, const std::string& saveDir) = delete;
	static bool SaveGame(const GameSaveData& saveData, const char * saveDir) = delete;
	static bool LoadSave(int saveId, GameSaveData& outSave, const std::string& saveDir) = delete;
	static bool LoadSave(int saveId, GameSaveData& outSave, const char * saveDir) = delete;
	static bool DeleteSave(int saveId, const std::string& saveDir) = delete;
	static bool DeleteSave(int saveId, const char * saveDir) = delete;
};

#endif // SAVEMANAGER
