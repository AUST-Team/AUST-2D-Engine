#include <SDL3/SDL.h>

#include "TypewriterEffect.h"
#include "FloatUtils.h"

TypewriterEffect::TypewriterEffect(const std::string& text, float delay) :
    fullText(text),
    delayMs(delay) {}

void TypewriterEffect::Update(double deltaTime) 
{
    if (finished)
    {
        return;
    }

    if (fullText.empty()) 
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "TypewriterEffect.Update: Text is empty.");
        finished = true;
        return;
    }

    timer += static_cast<float> (deltaTime);
    while (FloatUtils::IsGreaterOrEqualF(timer, delayMs) && currentIndex < fullText.size())
    {
        currentText += fullText[currentIndex++];
        timer -= delayMs;
    }

    if (currentIndex >= fullText.size()) 
    {
        finished = true;
    }
}

void TypewriterEffect::ForceFinish()
{
    if (finished)
    {
        return;
    }

    currentText = fullText;
    currentIndex = fullText.size();
    finished = true;
}

const std::string& TypewriterEffect::GetText() const { return currentText; }

bool TypewriterEffect::IsFinished() const { return finished; }


