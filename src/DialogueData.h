#pragma once

#ifndef DIALOGUEDATA_H_
#define DIALOGUEDATA_H_

#include <nlohmann/json.hpp>

#include "DialogueType.h"

/**
* @brief Structure for dialogue data.
*/
struct DialogueData
{
	nlohmann::json data;	/// The actual data, containing the JSON parameters.
	DialogueType type = DialogueType::ENUM_SENTINEL_VALUE;	/// The type of the dialogue.
};

#endif // DIALOGUEDATA_H_