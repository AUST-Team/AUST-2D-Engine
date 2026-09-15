#pragma once

#ifndef SCOREMANAGER_H_
#define SCOREMANAGER_H_

#include <string>
#include <vector>
#include <filesystem>

#include "Configuration.h"
#include "ScoreEntry.h"

/**
* @brief Class for a score manager, saves as '.csv'.
*/
class ScoreManager 
{
private:
    std::vector<ScoreEntry> scores;  /// Vector of the score entries.

    /**
    * @brief Saves the score to the file.
    * 
    * @param filePath The path to the file containing the scores.
    * 
    * @return 'true' if the scores have been saved, 'false' otherwise.
    */
    bool SaveToFile(const std::filesystem::path& filePath = Configuration::Get().paths.scoreFilePath);

    bool SaveToFile(const std::string& filePath) = delete;
    bool SaveToFile(const char * filePath) = delete;

public:

    /**
    * @brief Loads the scores from the file.
    *
    * @param filePath The path to the file containing the scores.
    *
    * @return 'true' if zero or more scores have been loaded, 'false' otherwise.
    */
    bool LoadScoresFromFile(const std::filesystem::path& filePath = Configuration::Get().paths.scoreFilePath);

    /**
    * @brief Loads the scores from a JSON.
    *
    * @param scoresJSON JSON containing the scores.
    *
    * @return 'true' if zero or more scores have been loaded, 'false' otherwise.
    */
    bool LoadScoresFromJSON(const nlohmann::json& scoresJSON);

    /**
    * @brief Adds a score to the vector.
    * 
    * @param name Name of the player.
    * @param timesCaught The number of times the player was caught.
    * @param timePlayedSeconds The seconds the player has played.
    * @param filePath The path to the file containing the scores.
    * 
    * @return 'true' if the score has been added succesfully, 'false' otherwise.
    */
    bool AddScore(const std::string& name, int timesCaught, uint64_t timePlayedSeconds, const std::filesystem::path& filePath = Configuration::Get().paths.scoreFilePath);

    /**
    * @brief Adds a score to the vector.
    *
    * @param entry Score entry.
    * @param filePath The path to the file containing the scores.
    *
    * @return 'true' if the score has been added succesfully, 'false' otherwise.
    */
    bool AddScore(const ScoreEntry& entry, const std::filesystem::path& filePath = Configuration::Get().paths.scoreFilePath);

    /**
    * @brief Returns the top 15 scores.
    *
    * @return A vector containing the score entries.
    */
    std::vector<ScoreEntry> GetTop15Scores() const;

    /**
    * @brief Clears the scores
    * 
    * @param filePath The path to the file containing the scores.
    * 
    * @return 'true' if the scores have been cleared, 'false' otherwise.
    */
    bool ClearScores(const std::filesystem::path& filePath = Configuration::Get().paths.scoreFilePath);

    bool LoadScoresFromFile(const std::string& filePath) = delete;
    bool LoadScoresFromFile(const char * filePath) = delete;
    bool AddScore(const std::string& name, int timesCaught, uint64_t timePlayedSeconds, const std::string& filePath) = delete;
    bool AddScore(const std::string& name, int timesCaught, uint64_t timePlayedSeconds, const char * filePath) = delete;
    bool AddScore(const ScoreEntry& entry, const std::string& filePath) = delete;
    bool AddScore(const ScoreEntry& entry, const char * filePath) = delete;
    bool ClearScores(const std::string& filePath) = delete;
    bool ClearScores(const char * filePath) = delete;
};

#endif // SCOREMANAGER_H_
