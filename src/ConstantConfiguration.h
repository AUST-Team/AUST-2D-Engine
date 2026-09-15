#pragma once

#ifndef CONSTANTCONFIGURATION_H_
#define CONSTANTCONFIGURATION_H_

#include <cstdint>
#include <filesystem>

using HashID = uint64_t;
#define ENUM_SENTINEL_VALUE Undefined
using SentinelEnumType = uint8_t;

/**
* @brief Namespace for configurations that shouldn't change.
*/
namespace ConstantConfiguration
{
    static constexpr HashID fnvOffsetBasis64 = 14695981039346656037ULL;   /// 64-bit FNV-1a offset basis.
    static constexpr HashID fnvPrime64 = 1099511628211ULL;    /// 64-bit FNV-1a prime.
    static constexpr HashID invalidHashId = 0;                /// ID for an invalid hash.
    static constexpr size_t bitsPerChunk = 64;	    /// Number of bits per chunk for the flags.
    static constexpr float epsilon = 1.0e-5f;       /// Epsilon value for mathematical computations.
    static constexpr int invalidTileIndex = -1;     /// Index for an invalid tile.

	static constexpr SentinelEnumType enumSentinelValue = 0;			/// Default value for undefined sentinel enum values.
	static const char* const enumSentinelString = "Undefined";		/// Default string for undefined sentinel enum values.
	static const char* const enumSentinelStringLower = "undefined";	/// Default string for undefined sentinel enum values in lowercase.

	static const std::filesystem::path defaultModsDirectory = std::filesystem::path("resources") / "mods";		/// Default directory for mods.
	static const char* const defaultGameDirectoryName = "aust";													/// Default name for the game.
	static const std::filesystem::path defaultGameDirectoryPath = std::filesystem::path("resources") / defaultGameDirectoryName;	/// Default path to the directory for the game.
	static const std::filesystem::path defaultLogDirectory = std::filesystem::path("logs") / "gameLogs";		/// Default directory for logs.

#ifdef _DEBUG
	static const std::filesystem::path defaultMemoryReportPath = std::filesystem::path("logs") / "memoryLogs";    /// Default path for memory reports.
	static const std::filesystem::path defaultVLDMemoryReportPath = std::filesystem::path("logs") / "VLDMLogs";   /// Default path for VLDM memory reports.
#endif // _DEBUG

	static const char* const isFullscreenValueKey = "__isFullscreen";		/// Default system value key for the 'isFullscreen' flag.
	static const char* const masterVolumeValueKey = "__masterVolume";		/// Default system value key for the master volume.
	static const char* const appWidthValueKey = "__appWidth";				/// Default system value key for the current width of the app.
	static const char* const appHeightValueKey = "__appHeight";				/// Default system value key for the current height of the app.
	static const char* const topScoresDataKey = "__topScores";				/// Default data provider key for top 15 secores.
	static const char* const playerCaughtDataKey = "__playerTimesCaught";	/// Default data provider for the number of times the player has been caught.
}

#define ENUM_SENTINEL_X_MACRO X(ENUM_SENTINEL_VALUE, ConstantConfiguration::enumSentinelValue, ConstantConfiguration::enumSentinelStringLower)

#endif // CONSTANTCONFIGURATION_H_