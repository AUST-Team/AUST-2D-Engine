#pragma once

#ifndef TYPEWRITEREFFECT_H_
#define TYPEWRITEREFFECT_H_

#include <string>

#include "Configuration.h"

/**
* @brief Creates a typewriter effect for text.
* 
* Yes it is necessary.
*/
class TypewriterEffect 
{
private:
    std::string fullText;       /// The full text to be displayed.
    std::string currentText;    /// The current text displayed.
    float delayMs = 500.0f;     /// Delay between each new character in miliseconds.
    float timer = 0.0f;         /// Timer.
    size_t currentIndex = 0;    /// Current character index.
    bool finished = false;      /// Flag if the effect has finished.

public:

    /**
    * @brief Sets-up the typewriter effect.
    * 
    * @param text Full text to be displayed.
    * @param delay Delay between each character.
    */
    TypewriterEffect(const std::string& text, float delay = 500.0f);

    /**
    * @brief Updates the typewriter effect.
    * 
    * @param deltaTime The deltaTime of the main SDL loop.
    */
    void Update(double deltaTime);

    /**
    * @brief Forces the typewritter effect to finish.
    */
    void ForceFinish();

    /**
    * @brief Returns the currently constructed text.
    * 
    * @return String of the current constructed text.
    */
    const std::string& GetText() const;

    /**
    * @brief Returns if the effect has finished.
    * 
    * @return true if the effect has finished, false otherwise.
    */
    bool IsFinished() const;
};

#endif // TYPEWRITEREFFECT_H_