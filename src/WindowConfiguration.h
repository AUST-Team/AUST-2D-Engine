#pragma once

#ifndef WINDOWCONFIGURATION_H_
#define WINDOWCONFIGURATION_H_

#include <string>

/**
* @brief Structure for window configuration.
* 
* Although struct Size exists, this is here for an eventual additions to the window.
*/
struct WindowConfiguration 
{
    std::string iconFileName = "flagIcon.png";  /// Name of the window icon.
    std::string name = "Game";  /// Name of the window.
    int width = 800;   /// Width of the window.
    int height = 600;  /// Height of the window.
};

#endif // WINDOWCONFIGURATION_H_