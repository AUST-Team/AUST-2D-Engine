#include <SDL3/SDL_log.h>
#include <SDL3/SDL_rect.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>
#include <sstream>

#include "StringMiscs.h"

#include "SettingsManager.h"

std::string StringMiscs::ToLower(std::string str)
{
	std::transform(
		str.begin(),	// Start index.
		str.end(),		// End index.
		str.begin(),	// Output index.
        [](unsigned char c) -> char {
            return static_cast<char>(std::tolower(c));
        }   // Function to apply.
	);
	return str;
}

std::string StringMiscs::CapitalizeFirst(std::string str)
{
	if (!str.empty())
	{
		str[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(str[0])));
	}
	return str;
}

bool StringMiscs::IsStringEnumSentinel(const std::string& str, const std::string& sentinelStr) { return ToLower(str) == ToLower(sentinelStr); }

std::vector<std::string> StringMiscs::WrapText(const std::string& fullText, const SDL_FRect& bounds, TTF_Font* font, float padding)
{
    std::vector<std::string> result;

    if (!SettingsManager::GetInstance().CheckSupressWrapTextWarning() && fullText.empty())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "StringMiscs.WrapText: Text is null.");
        return result;
    }

    if (!font)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "StringMiscs.WrapText: Font is null for %s.", fullText.c_str());
        return result;
    }

    std::istringstream stream(fullText);    // Automatically breaks the text into words using white spaces.
    std::string word;
    std::string line;
    int maxWidth = static_cast<int>(std::round(bounds.w - padding));

    while (stream >> word)
    {
        // Get the first word or concatenate the next word.
        std::string testLine = line.empty() ? word : line + " " + word;
        int width = 0;

        if (!TTF_GetStringSize(font, testLine.c_str(), 0, &width, nullptr))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "StringMiscs.WrapText: Failed GetStringSize for %s: %s.", fullText.c_str(), SDL_GetError());
            continue;
        }

        // If the width is bigger than the maxWidth, push the line and the put the word on a new line.
        // Else, concatenate the word to the actual line.
        if (width > maxWidth)
        {
            result.push_back(line);
            line = word;
        }
        else
        {
            line = testLine;
        }
    }

    if (!line.empty())
    {
        result.push_back(line);
    }

    return result;
}

std::string StringMiscs::TruncateTextToFit(const std::string& fullText, TTF_Font* font, int maxWidth)
{
    if (fullText.empty())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "StringMiscs.TruncateTextToFit: Text is empty.");
        return fullText;
    }

    if (!font)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "StringMiscs.TruncateTextToFit: Font is null for %s", fullText.c_str());
        return fullText;
    }

    int width = 0;

    if (!TTF_GetStringSize(font, fullText.c_str(), 0, &width, nullptr))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "StringMiscs.TruncateTextToFit: Failed GetStringSize for %s: %s", fullText.c_str(), SDL_GetError());
        return fullText;
    }

    if (width <= maxWidth)
    {
        return fullText;
    }

    std::string truncated = fullText;
    constexpr const char* const ellipsis = "...";

    while (!truncated.empty())
    {
        truncated.pop_back();
        std::string testStr = truncated + ellipsis;

        if (!TTF_GetStringSize(font, testStr.c_str(), 0, &width, nullptr))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "StringMiscs.TruncateTextToFit: Failed GetStringSize for %s: %s", fullText.c_str(), SDL_GetError());
            return fullText;
        }

        if (width <= maxWidth)
        {
            return testStr;
        }
    }

    return "";
}
