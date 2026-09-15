#pragma once

#ifndef GAMETIME_H_
#define GAMETIME_H_

#include <chrono>

/**
* @brief Static helper class for tracking the amount of time the player has spent playing.
* 
* Starts when the actual game begins.
*/
class GameTime 
{
private:
    static std::chrono::steady_clock::time_point startTime; /// The start time.
    static uint64_t totalPlayTimeSeconds;       /// Total playtime in seconds.
	static uint64_t totalPlayTimeMiliseconds;   /// Total playtime in milliseconds.
    static bool trackingFlag;   /// Flag if currently tracking or not.

    // No instancing due to static helper class.
    GameTime() = delete;
    ~GameTime() = delete;

    // No copying or moving due to static helper class.
    GameTime(const GameTime&) = delete;
    GameTime& operator=(const GameTime&) = delete;
    GameTime(GameTime&&) = delete;
    GameTime& operator=(GameTime&&) = delete;

public:
    
    /**
    * @brief Starts tracking the time played.
    */
    static void StartNewTracking();

    /**
    * @brief Continues the tracking from previous time.
    * 
    * @param savedTimeSeconds The amount of seconds previously played.
    */
    static void ContinueTracking(uint64_t savedTimeSeconds);

    /**
    * @brief Stops tracking.
    */
    static void StopTracking();

    /**
    * @brief Resets tracking.
    */
    static void Reset();

    /**
    * @brief Returns a snapshot of the current total play time.
    * 
    * Does not stop tracking.
    * 
    * @return The total play time in seconds at the time of the snapshot.
    */
    static uint64_t PeekPlayTimeSeconds();

    /**
	* @brief Returns a snapshot of the current total play time in milliseconds.
    * 
    * Does not stop tracking.
    * 
	* @return The total play time in milliseconds at the time of the snapshot.
    */
	static uint64_t PeekPlayTimeMiliseconds();

    /**
    * @brief Returns the total play time in seconds.
    * 
    * @return The total play time in seconds.
    */
    static uint64_t GetPlayTimeSeconds();

    /**
	* @brief Returns the total play time in milliseconds.
    * 
	* @return The total play time in milliseconds.
    */
	static uint64_t GetPlayTimeMiliseconds();
    
    /**
    * @brief Formats the seconds as 'MM:SS'
    * 
    * @return A string containing the formatted time.
    */
    static std::string FormatTime(uint64_t totalSeconds);
}; 

#endif // GAMETIME_H_