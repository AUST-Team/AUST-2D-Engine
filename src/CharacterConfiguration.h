#pragma once

#ifndef CHARACTERCONFIGURATION_H_
#define CHARACTERCONFIGURATION_H_

/**
* @brief Structure for character (entity) configuration.
*/
struct CharacterConfiguration
{
    int width = 48;     /// Width (in pixels) of a character.
    int height = 48;    /// Height (in pixels) of a character.
    float speed = 3.0f; /// Speed (in pixels) of a character.
};

#endif // CHARACTERCONFIGURATION_H_