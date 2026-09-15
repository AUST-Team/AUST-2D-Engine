#pragma once

#ifndef SCOREENTRY_H_
#define SCOREENTRY_H_

#include <string>

/**
* @brief Structure for a score entry.
*/
struct ScoreEntry
{
	std::string name;			/// Name of the player.
	int timesCaught = 0;		/// The times the player has been caught.
	uint64_t timePlayed = 0;	/// Time played (seconds).
};

#endif // SCOREENTRY_H_