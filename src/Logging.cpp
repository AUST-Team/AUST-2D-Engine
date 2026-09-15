#include "Logging.h"

#if defined(USE_LOGGING) && LOG_OUTPUT > 0

#include <filesystem>
#include <iostream>
#include <fstream>

#include "ConstantConfiguration.h"
#include "SettingsManager.h"
#include "Unused.h"
#include "MemoryTracker.h"

static std::ofstream* logFileStream = nullptr;	/// Out-file stream for logging to file.

/**
* @brief Custom log output function that logs messages to both the console and a file. This function is set as the log output function for SDL.
*
* @param userdata A pointer to user data. Can be anything.
* @param category The category of the log message.
* @param priority The priority of the log message.
* @param message The log message to output.
*/
static void CustomLogOutput(void* userdata, int category, SDL_LogPriority priority, const char* message)
{
	UNUSED(category);

	if (!SettingsManager::GetInstance().CheckUseLogging())
	{
		return;
	}

	const std::string prefix = Logging::GetPriorityString(priority);
	const std::string formattedMessage = prefix + " " + message;

#if LOG_OUTPUT & LOG_TO_FILE

	if (SettingsManager::GetInstance().CheckLogToFile())
	{
		std::ofstream* logFile = static_cast<std::ofstream*>(userdata);
		if (logFile && logFile->is_open())
		{
			(*logFile) << formattedMessage << std::endl;
		}
	}

#else // !(LOG_OUTPUT & LOG_TO_FILE)

	UNUSED(userdata);

#endif // LOG_OUTPUT & LOG_TO_FILE

#if LOG_OUTPUT & LOG_TO_CONSOLE

	if (SettingsManager::GetInstance().CheckLogToConsole())
	{
		if (priority >= SDL_LOG_PRIORITY_WARN)
		{
			std::cerr << formattedMessage << std::endl;
		}
		else
		{
			std::cout << formattedMessage << std::endl;
		}
	}

#endif // LOG_OUTPUT & LOG_TO_CONSOLE

}

namespace Logging
{
	void Init()
	{
		bool createLogFile = false;

#if LOG_OUTPUT & LOG_TO_FILE

		if (SettingsManager::GetInstance().CheckLogToFile())
		{
			createLogFile = true;
		}
#endif

		if (createLogFile)
		{
			std::error_code ec;
			std::filesystem::create_directories(ConstantConfiguration::defaultLogDirectory, ec);

			if (ec)
			{
				SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Logging.Init: Failed to create directories: %s", ec.message().c_str());
			}

			std::chrono::system_clock::time_point t = std::chrono::system_clock::now();
			const time_t tt = std::chrono::system_clock::to_time_t(t);
			std::tm local_tm;
			localtime_s(&local_tm, &tt);

			std::stringstream ss;
			ss << std::put_time(&local_tm, "%Y%m%d_%H%M%S");
			std::string timestamp = ss.str();

			std::filesystem::path reportPath = std::filesystem::path(ConstantConfiguration::defaultLogDirectory) / ("GameLog_" + timestamp + ".log");

			// Apparently this works with stuff like this too.
			logFileStream = ENG_NEW(std::ofstream, reportPath);
			if (!logFileStream->is_open())
			{
				SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Logging.Init: Failed to create game report file at: %s", reportPath.string().c_str());
				ENG_DELETE(logFileStream);
				logFileStream = nullptr;
			}
		}

		SDL_SetLogOutputFunction(CustomLogOutput, static_cast<void*>(logFileStream));
	}

	void Cleanup()
	{
		if (logFileStream)
		{
			if (logFileStream->is_open())
			{
				logFileStream->close();
			}
			ENG_DELETE(logFileStream);
			logFileStream = nullptr;
		}
	}
	
	const char* GetPriorityString(SDL_LogPriority priority)
	{
		switch (priority)
		{
			case SDL_LOG_PRIORITY_VERBOSE:  return "VERBOSE: ";
			case SDL_LOG_PRIORITY_DEBUG:    return "DEBUG:   ";
			case SDL_LOG_PRIORITY_INFO:     return "INFO:    ";
			case SDL_LOG_PRIORITY_WARN:     return "WARNING: ";
			case SDL_LOG_PRIORITY_ERROR:    return "ERROR:   ";
			case SDL_LOG_PRIORITY_CRITICAL: return "CRITICAL:";
			default:                        return "LOG:     ";
		}
	}
}

#endif // defined(USE_LOGGING) && LOG_OUTPUT > 0