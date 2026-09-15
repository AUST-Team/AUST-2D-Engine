#include <SDL3/SDL_log.h>

#include "DataProvider.h"

#include "ConstantConfiguration.h"
#include "ScoreManager.h"
#include "GameTime.h"

void DataProvider::InitialiseProviders()
{
    RegisterProvider(ConstantConfiguration::topScoresDataKey, []() {
        ScoreManager scoreManager;
        scoreManager.LoadScoresFromFile();
        std::vector<ScoreEntry> topScores = scoreManager.GetTop15Scores();

        std::vector<std::vector<std::string>> data;
        for (const ScoreEntry& entry : topScores)
        {
            data.push_back({ entry.name, std::to_string(entry.timesCaught), GameTime::FormatTime(entry.timePlayed) });
        }
        return data;
    });

    RegisterProvider(ConstantConfiguration::playerCaughtDataKey, []() {
        return -1;
    });
}

void DataProvider::RegisterProvider(const std::string& key, const ProviderEntry& provider) { providers[key] = provider; }

void DataProvider::RegisterProvider(const std::string& key, const ProviderFunc& func, ProviderLifetime lifetime)
{
    const ProviderEntry provider {
        .func = func,
        .lifetime = lifetime
    };
    RegisterProvider(key, provider);
}
