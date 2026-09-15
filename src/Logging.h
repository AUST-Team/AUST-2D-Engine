#pragma once

#ifndef LOGGING_H_
#define LOGGING_H_

#define USE_LOGGING

#if defined(USE_LOGGING)

#include <SDL3/SDL_log.h>

// Never ever in my LIFE will I convert a macro to a CONSTEXPR if I don't need type checking.

#define LOG_TO_CONSOLE	1 << 0
#define LOG_TO_FILE		1 << 1

#define LOG_OUTPUT ((LOG_TO_FILE) | (LOG_TO_CONSOLE))

/**
* @brief Namespace for logging-related functions, such as initialization and custom log output handling.
*/
namespace Logging
{
	/**
	* @brief Initializes the logging system. All this does is make SDL log to the console and also to a folder. if set to do so.
	*/
	void Init();

	/**
	* @brief Cleans up resources used by the logging.
	*/
	void Cleanup();

	/**
	* @brief Helper function to convert SDL_LogPriority to a string representation for better readability in logs.
	* 
	* @param priority The SDL_LogPriority to convert.
	* 
	* @return A string representation of the log priority.
	*/
	const char* GetPriorityString(SDL_LogPriority priority);
}

#define LOGGING_INIT() Logging::Init()
#define LOGGING_CLEANUP() Logging::Cleanup()

#else // !defined(USE_LOGGING)

#define LOGGING_INIT() ((void)0)
#define LOGGING_CLEANUP() ((void)0)

#endif // defined(USE_LOGGING)

#endif // LOGGING_H_