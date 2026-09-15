#include "GameTime.h"

std::chrono::steady_clock::time_point GameTime::startTime;
uint64_t GameTime::totalPlayTimeSeconds = 0;
bool GameTime::trackingFlag = false;
uint64_t GameTime::totalPlayTimeMiliseconds = 0;

void GameTime::StartNewTracking() 
{
    startTime = std::chrono::steady_clock::now();
    totalPlayTimeSeconds = 0;
	totalPlayTimeMiliseconds = 0;
    trackingFlag = true;
}

void GameTime::ContinueTracking(uint64_t savedTimeSeconds)
{
    startTime = std::chrono::steady_clock::now();
    totalPlayTimeSeconds = savedTimeSeconds;
	totalPlayTimeMiliseconds = savedTimeSeconds * 1000;
    trackingFlag = true;
}

void GameTime::StopTracking() 
{
    if (!trackingFlag)
    {
        return;
    }

    const std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
    const long long elapsed = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
    totalPlayTimeSeconds += static_cast<int>(elapsed);
	totalPlayTimeMiliseconds += static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count());
    trackingFlag = false;
}

void GameTime::Reset() 
{
    totalPlayTimeSeconds = 0;
	totalPlayTimeMiliseconds = 0;
    trackingFlag = false;
}

uint64_t GameTime::PeekPlayTimeSeconds()
{
    if (!trackingFlag)
    {
        return totalPlayTimeSeconds;
    }

    const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    const long long elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
    return totalPlayTimeSeconds + static_cast<unsigned long int>(elapsed);
}

uint64_t GameTime::PeekPlayTimeMiliseconds()
{
    if(!trackingFlag)
    {
        return totalPlayTimeMiliseconds;
	}

    const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    const long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
	return totalPlayTimeMiliseconds + static_cast<unsigned long int>(elapsed);
}

uint64_t GameTime::GetPlayTimeSeconds() { return totalPlayTimeSeconds; }

uint64_t GameTime::GetPlayTimeMiliseconds() { return totalPlayTimeMiliseconds; }

std::string GameTime::FormatTime(uint64_t totalSeconds)
{
    const uint64_t minutes = totalSeconds / 60;
    const uint64_t seconds = totalSeconds % 60;

    std::string result;

    if (minutes < 10)
    {
        result += "0";
    }

    result += std::to_string(minutes);
    result += ":";

    if (seconds < 10)
    {
        result += "0";
    }
    result += std::to_string(seconds);

    return result;
}
