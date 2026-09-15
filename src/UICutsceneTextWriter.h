#pragma once

#ifndef UICUTSCENETEXTWRITER_H_
#define UICUTSCENETEXTWRITER_H_

#include <vector>
#include <functional>

#include "UIElement.h"
#include "UITextStyleElement.h"
#include "TypewriterEffect.h"
#include "UILabelPool.h"
#include "WriterState.h"
#include "WriterStateType.h"

/**
* @brief An UI element that writes lines on screen, pauses, and writes the next one.
*
* Fades out at the end.
*/
class UICutsceneTextWriter : public UIElement, public UITextStyleElement
{
private:
    std::vector<std::string> lines = {}; /// All the lines to be written.
    std::function<void()> onComplete = nullptr;   /// Function to call on complete.
    UILabel* skipLabel = nullptr;   /// Pointer to the skip label. NON-OWNING.
    TypewriterEffect typewriter;    /// Typewriter effect.
    UILabelPool cachedLabels;   /// Pool of cached labels.
    std::unordered_map<WriterStateType, WriterState> writerStates;  /// Map of [stateType; state] for the cutscene writer.

    int lineIndex = 0;          /// Index for which line we are currently on.
    float timeForPause = 2000.0f;   /// Time for the pauses in between lines.
    float typewriterSpeed = 40.0f;  /// Typewriter speed.
    float pauseTimer = 0.0f;    /// Timer for the pause.
    float fadeSpeed = 0.01f;    /// Speed of fading.
    float fadeOpacity = 1.0f;   /// Current opacity.
    bool completed = false;     /// Flag if completed.
    WriterStateType currentState = WriterStateType::Typing;    /// Current writer state.

    // No copying due to the label pool being uncopyable (cached textures).
    UICutsceneTextWriter(const UICutsceneTextWriter&) = delete;
    UICutsceneTextWriter& operator=(const UICutsceneTextWriter&) = delete;

    /**
    * @brief Ticks the typewriter.
    * 
    * @param DeltaTime The delta time of the main game loop.
    */
    void TickTypewriter(double deltaTime);

    /**
    * @brief Advances to the next line in the vector.
    */
    void AdvanceLine();

    /**
    * @brief Starts the fade out.
    */
    void StartFadeOut();

    /**
    * @brief Updates the cached labels.
    * 
    * @param renderer Pointer to the renderer used.
    */
    void UpdateLabelsCache();

    /**
    * @brief Instantly finished the current line.
    */
    void InstantlyFinishCurrentLine();

protected:

    /**
    * @brief Renders the cutscene.
    *
    * @param renderer Pointer to the renderer used.
    */
    void OnRender(SDL_Renderer* renderer) const override;

public:

    /**
    * @brief Constructor.
    * 
    * @param id ID of the element.
    * @param bounds The bounds of the cutscene.
    * @param backgroundColor Background color of the writer.
    * @param borderColor Border color of the writer.
    * @param textFont Font of the text.
    * @param textColor Color of the text.
    * @param textAlignment Alignment of the text.
	* @param fontFamily Font family name of the text font.
    * @param pauseTime Time between finishing a line and starting the next one.
    * @param fadeSpeed The fade speed of the fadeout. Must be positive!
    * @param typewriterSpeed The typewriting speed of the typewriter. Lower -> faster.
    * @param lines All the lines of the cutscene.
    */
    explicit UICutsceneTextWriter(
        const std::string id = "",
        const SDL_FRect& bounds = {0.0f, 0.0f, 0.0f, 0.0f},
        const SDL_Color& backgroundColor = { 0, 0, 0, 0 },
        const SDL_Color& borderColor = { 255, 255, 255, 255 },
        TTF_Font* textFont = nullptr,
        const SDL_Color& textColor = { 255, 255, 255, 255},
        const UITextAlignment& textAlignment = { HorizontalAlignment::Center, VerticalAlignment::Middle },
		const std::string& fontFamily = "",
        float pauseTime = 2000.f,
        float fadeSpeed = 0.01f,
        float typewriterSpeed = 40.0f,
        const std::vector<std::string>& lines = {}
    );

    /**
    * @brief Default destructor.
    */
    ~UICutsceneTextWriter() override = default;

    /**
    * @brief Handles an event.
    * 
    * @param e Event to be handled.
    */
    void HandleEvent(const SDL_Event& e) override;

    /**
    * @brief Updates the cutscene.
    * 
    * @param deltaTime The delta time from the main SDL loop.
    */
    void Update(double deltaTime) override;

    /**
    * @brief Resizes the element.
    *
    * @param widthScale The scale of the width (newWidth / oldWidth).
    * @param heightScale The scale of the height (newHeight / oldHeight).
    */
    void Resize(float widthScale, float heightScale) override;

    /**
    * @brief Sets the function to call when the cutscene completes.
    *
    * @param callback Function to call once cutscene finishes.
    */
    void SetOnComplete(std::function<void()> callback);

    /**
    * @brief Checks if the cutscene is complete (returns isComplete flag).
    * 
    * @return 'true' if the cutscene is complete, false if otherwise.
    */
    bool IsComplete() const;

    /**
    * @brief Checks if the cutscene is fading out (returns fadingOut flag).
    *
    * @return 'true' if the cutscene is fading out, false if otherwise.
    */
    bool IsFadingOut() const;

    /**
    * @brief Sets the bounds of the cutscene writer.
    * 
    * @param newBounds New bounds of the writer.
    */
    void SetBounds(const SDL_FRect& newBounds) override;

    /**
    * @brief Sets the background color of the writer.
    *
    * @param newColor New background color.
    */
    void SetBackgroundColor(const SDL_Color& newColor) override;

    /**
    * @brief Sets the text color of the writer.
    *
    * @param newColor New text color.
    */
    void SetTextColor(const SDL_Color& newColor) override;

    /**
    * @brief Sets the text font of the writer.
    * 
    * @param newFont New font of the text.
    */
    void SetTextFont(TTF_Font* newFont) override;

    /**
    * @brief Sets the text alignment of the writer.
    * 
    * @param newAlignment New alignment of the text.
    */
    void SetTextAlignment(const UITextAlignment& newAlignment) override;

    /**
    * @brief Sets the new skip label.
    * 
    * @param newLabel Pointer to the new label. NON-OWNING.
    */
    void SetSkipLabel(UILabel* newLabel);

    /**
    * @brief Returns the skip label.
    * 
    * @return Pointer to the skip label, or nullptr, if no label has been set.
    */
    UILabel* GetSkipLabel() const;
};
#endif // UICUTSCENETEXTWRITER_H_