#pragma once

#ifndef PLAYERCONFIGURATION_H_
#define PLAYERCONFIGURATION_H_

/**
* @brief Structure for player configuration.
*/
struct PlayerConfiguration
{
    int maxTimesCaught = 3; /// Maximum times the player can be caught before automatically triggering the bad end.
};

#endif // PLAYERCONFIGURATION_H_