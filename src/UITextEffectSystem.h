#pragma once

#ifndef UITEXTEFFECTSYSTEM_H_
#define UITEXTEFFECTSYSTEM_H_

#include <unordered_map>
#include <string>

#include "Configuration.h"

class TypewriterEffect;

/**
* @brief Keeps all the text effects in one place (well typewriter since that's the only thing I want).
*/
class UITextEffectSystem 
{
private:

    /**
    * @brief Structure for a typewriter effect entry.
    * 
    * Needed so it can be destroyed safely, and have access to the text for one update cycle after finishing.
    */
    struct TypewriterEntry
    {
        TypewriterEffect* effect = nullptr; /// The typewriter effect.
        bool destroy = false;               /// If the pointer can be destroyed or not.
    };

    std::unordered_map<std::string, TypewriterEntry> effects; /// Maps a name to a typewriter effect.

    /**
    * @brief Default constructor.
    */
    UITextEffectSystem() = default;

    /**
    * @brief Destructor.
    */
    ~UITextEffectSystem();

    // No copying or moving allowed due to singleton pattern.
    UITextEffectSystem(const UITextEffectSystem&) = delete;
    UITextEffectSystem& operator=(const UITextEffectSystem&) = delete;
    UITextEffectSystem(UITextEffectSystem&&) = delete;
    UITextEffectSystem& operator=(UITextEffectSystem&&) = delete;

public:

    /**
    * @brief Returns the instance of the UI text effect system.
    * 
    * @return Reference to the text effect system.
    */
    static UITextEffectSystem& GetInstance() noexcept
    {
        static UITextEffectSystem instance;
        return instance;
    }

    /*
    * @brief Adds an effect to the map.
    * 
    * @param id String that identifies the effect.
    * @param text Text to be typewritten.
    * @param delay Delay between characters.
    */
    void AddEffect(const std::string& id, const std::string& text, float delay = 50.0f);

    /**
    * @brief Updates all the text effects.
    * 
    * @param deltaTime The delta time of the main SDL loop.
    */
    void Update(double deltaTime);

    /**
    * @brief Forces an effect to finish.
    * 
    * @id The name of the effect to force finish.
    */
    void ForceFinish(const std::string& id);

    /**
    * @brief Forces all effects to finish.
    */
    void ForceFinishAll();

    /**
    * @brief Clears all effects.
    */
    void Clear();

    /**
    * @brief Returns the current text of an effect.
    * 
    * @param id The string associated with the effect.
    */
    std::string GetText(const std::string& id) const;

    /**
    * @brief Checks if there is an effect mapped to a certain id.
    * 
    * @param id The name to check if any effect is mapped to.
    * 
    * @return true if there is an effect mapped to 'id', false otherwise.
    */
    bool HasEffect(const std::string& id) const;

    /**
    * @brief Checks if a certain effect has finished.
    * 
    * @param id The name that the effect is mapped to.
    * 
    * @return true if the effect mapped to 'id' has finished, false otherwise.
    */
    bool IsFinished(const std::string& id) const;
};

#endif // UITEXTEFFECTSYSTEM_H_
