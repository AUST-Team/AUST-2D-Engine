#pragma once

#ifndef APPLICATION_H_
#define APPLICATION_H_

/**
* @brief Facade for the program.
*/
namespace Application
{
    /**
    * @brief Initialises the SDL system and renderer.
    *
	* @param argc Argument count from main.
	* @param argv Argument vector from main.
    * 
    * @return true if all initialisations were successful, false otherwise.
    */
    bool Init(int argc, char* argv[]);

    /**
    * @brief Starts the game (main menu).
    */
    void Run();

    /**
    * @brief Cleans up any resources.
    */
    void Cleanup();
};

#endif // APPLICATION_H_