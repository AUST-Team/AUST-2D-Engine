#pragma once

#ifndef APPCONFIGURATION_H_
#define APPCONFIGURATION_H_

/**
* @brief App configuration structure.
*/
struct AppConfiguration
{
    int targetFPS = 60;    /// Target FPS of the game.
    double frameDelayMs = 1000.0 / targetFPS;   /// Delay between frames.
};

#endif // APPCONFIGURATION_H_