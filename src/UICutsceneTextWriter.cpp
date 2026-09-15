#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "UICutsceneTextWriter.h"

#include "Miscs.h"
#include "AppRenderSystem.h"
#include "MemoryTracker.h"
#include "StringMiscs.h"
#include "Unused.h"
#include "FontManager.h"

UICutsceneTextWriter::UICutsceneTextWriter(const std::string id, const SDL_FRect& bounds, const SDL_Color& backgroundColor, const SDL_Color& borderColor, TTF_Font* textFont, const SDL_Color& textColor, const UITextAlignment& textAlignment, const std::string& fontFamily, float pauseTime, float fadeSpeed, float typewriterSpeed, const std::vector<std::string>& lines) :
    UIElement(id, bounds, backgroundColor, borderColor),
    UITextStyleElement(textFont, textColor, textAlignment, fontFamily),
    timeForPause(pauseTime),
    fadeSpeed(fadeSpeed),
    lines(lines),
    typewriterSpeed(typewriterSpeed),
    typewriter(lines.empty() ? "" : lines[0], typewriterSpeed) 
{
    writerStates.emplace(WriterStateType::Typing, WriterState {
        .onUserInteract = [this](uint32_t type, uint32_t key) {
            if (type == SDL_EVENT_MOUSE_BUTTON_DOWN && key == SDL_BUTTON_LEFT)
            {
                StartFadeOut();
            }
            else if (type == SDL_EVENT_KEY_DOWN || type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                InstantlyFinishCurrentLine();
                currentState = WriterStateType::Waiting;
            }
        },
        .onUpdate = [this](double deltaTime) {
            TickTypewriter(deltaTime);
        }
    });

    writerStates.emplace(WriterStateType::Waiting, WriterState {
        .onUserInteract = [this](uint32_t type, uint32_t key) {
            if (type == SDL_EVENT_MOUSE_BUTTON_DOWN && key == SDL_BUTTON_LEFT)
            {
                StartFadeOut();
            }
            else if (type == SDL_EVENT_KEY_DOWN || type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                AdvanceLine();
            }
        },
        .onUpdate = [this](double deltaTime) {
            pauseTimer += static_cast<float>(deltaTime);
            if (pauseTimer >= timeForPause)
            {
                AdvanceLine();
            }
        }
    });

    writerStates.emplace(WriterStateType::FadingOut, WriterState{
        .onUserInteract = nullptr,
        .onUpdate = [this](double deltaTime) {
            fadeOpacity -= static_cast<float>(this->fadeSpeed * deltaTime);
            if (fadeOpacity <= 0.0f)
            {
                fadeOpacity = 0.0f;
                currentState = WriterStateType::Finished;
            }
            TickTypewriter(deltaTime);
        }
    });

    writerStates.emplace(WriterStateType::Finished, WriterState{
        .onUserInteract = nullptr,
        .onUpdate = [this](double deltaTime) {
            UNUSED(deltaTime);
            if (!completed && onComplete)
            {
                completed = true;
                onComplete();
            }
        }
    });
}

void UICutsceneTextWriter::HandleEvent(const SDL_Event& e)
{
    std::function<void(uint32_t, uint32_t)>& onInteractFunc = writerStates[currentState].onUserInteract;
    if (onInteractFunc)
    {
        onInteractFunc(e.type, e.type == SDL_EVENT_MOUSE_BUTTON_DOWN ? e.button.button : e.key.key);
    }
}

void UICutsceneTextWriter::Update(double deltaTime)
{
    std::function<void(double)>& onUpdateFunc = writerStates[currentState].onUpdate;
    if (onUpdateFunc)
    {
        onUpdateFunc(deltaTime);
    }
}

void UICutsceneTextWriter::Resize(float widthScale, float heightScale) 
{ 
    UIElement::Resize(widthScale, heightScale); 

    float fontScale = std::min(widthScale, heightScale);
    float newFontSize = originalFontSize * fontScale;

    TTF_Font* newFont = FontManager::GetInstance().GetFontByFamily(fontFamily, newFontSize);

    if (!newFont)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UICutsceneTextWriter.Resize: Failed to load new font: %s", SDL_GetError());
        return;
    }

    SetTextFont(newFont);
    cachedLabels.SetTextFont(newFont);
}

void UICutsceneTextWriter::SetOnComplete(std::function<void()> callback) { onComplete = callback; }

bool UICutsceneTextWriter::IsComplete() const { return completed; }

bool UICutsceneTextWriter::IsFadingOut() const { return currentState == WriterStateType::FadingOut; }

void UICutsceneTextWriter::SetBounds(const SDL_FRect& newBounds)
{
    UIElement::SetBounds(newBounds);
    UpdateLabelsCache();
}

void UICutsceneTextWriter::SetBackgroundColor(const SDL_Color& newColor)
{
    UIElement::SetBackgroundColor(newColor);
    UpdateLabelsCache();
}

void UICutsceneTextWriter::SetTextColor(const SDL_Color& newColor)
{
    UITextStyleElement::SetTextColor(newColor);
    UpdateLabelsCache();
}

void UICutsceneTextWriter::SetTextFont(TTF_Font* newFont)
{
    UITextStyleElement::SetTextFont(newFont);
    UpdateLabelsCache();
}

void UICutsceneTextWriter::SetTextAlignment(const UITextAlignment& newAlignment)
{
    UITextStyleElement::SetTextAlignment(newAlignment);
    UpdateLabelsCache();
}

void UICutsceneTextWriter::SetSkipLabel(UILabel* newLabel) { skipLabel = newLabel; }

UILabel* UICutsceneTextWriter::GetSkipLabel() const { return skipLabel; }

void UICutsceneTextWriter::TickTypewriter(double deltaTime)
{
    typewriter.Update(deltaTime);

    if (typewriter.IsFinished() && !completed)
    {
        if (cachedLabels.Empty())
        {
            // Build cached labels only once after finished typing.
            UpdateLabelsCache();
            currentState = WriterStateType::Waiting;
        }
    }
    else
    {
        // Clear cached labels during typing for raw rendering (typewriting).
        cachedLabels.Clear();
    }
}

void UICutsceneTextWriter::AdvanceLine()
{
    pauseTimer = 0.0f;

    if (++lineIndex < lines.size())
    {
        typewriter = TypewriterEffect(lines[lineIndex], typewriterSpeed);
        cachedLabels.Clear();   // If you clear these without creating a new typewriter effect, the program will crash.
        currentState = WriterStateType::Typing;
    }
    else
    {
        StartFadeOut();
    }
}

void UICutsceneTextWriter::StartFadeOut() 
{ 
    currentState = WriterStateType::FadingOut;

    if (skipLabel)
    {
        skipLabel->SetVisible(HIDE_ELEMENT);
    }
}

void UICutsceneTextWriter::UpdateLabelsCache()
{
    if (!textFont)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UICutsceneTextWriter.UpdateLabelsCache: Font is null.");
        return;
    }

    const std::vector<std::string> wrapped = StringMiscs::WrapText(lines[lineIndex], bounds, textFont);

    cachedLabels.Resize(wrapped.size());

    size_t fontHeight = currentFontHeight;
    size_t totalHeight = wrapped.size() * fontHeight;
    float startY = bounds.y + (bounds.h - totalHeight) / 2.0f;

    cachedLabels.SetTextColor(textColor);
    cachedLabels.SetTextAlignment(textAlignment);
    cachedLabels.SetBorderColor(SDL_Color { 0, 0, 0, 0 });

    for (size_t i = 0; i < wrapped.size(); ++i)
    {
        UILabel& lbl = cachedLabels[i];

        SDL_FRect lblBounds {
            bounds.x,
            startY + i * fontHeight,
            bounds.w,
            static_cast<float>(fontHeight)
        };

        lbl.SetBounds(lblBounds);
        lbl.SetText(wrapped[i]);
        lbl.OverrideTextFont(textFont, fontFamily);
        lbl.InvalidateCache();
    }
}

void UICutsceneTextWriter::InstantlyFinishCurrentLine()
{
    typewriter.ForceFinish();
    currentState = WriterStateType::Waiting;
    if (cachedLabels.Empty())
    {
        // Build cached labels only once after finished typing.
        UpdateLabelsCache();
    }
}

void UICutsceneTextWriter::OnRender(SDL_Renderer* renderer) const
{
    if (!textFont)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UICutsceneTextWriter.OnRender: Font is null.");
        return;
    }

    SDL_Color renderColor = textColor;
    renderColor.a = static_cast<uint8_t>(fadeOpacity * renderColor.a);

    if (typewriter.IsFinished() && currentState == WriterStateType::Waiting)
    {
        // Render cached labels.
        for (size_t i = 0; i < cachedLabels.Size(); ++i)
        {
            const UILabel& label = cachedLabels[i];
            if (!SDL_SetTextureAlphaMod(label.GetCachedTexture(), renderColor.a))
            {
				SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "UICutsceneTextWriter.OnRender: Failed SetTextureAlphaMod for cached label: %s.", SDL_GetError());
            }
            label.Render(renderer);
        }
    }
    else
    {
        // Render raw text during typing.
        std::string rawText = typewriter.GetText().empty() ? " " : typewriter.GetText();

        const std::vector<std::string>& wrapped = StringMiscs::WrapText(rawText, bounds, textFont);

        size_t fontHeight = currentFontHeight;
        size_t totalHeight = wrapped.size() * fontHeight;            // Total height (~ bounding box) of the text.
        float startY = bounds.y + (bounds.h - totalHeight) / 2.0f;  // Center the text in the middle of the screen (bounding box).

        for (size_t i = 0; i < wrapped.size(); ++i)
        {
            SDL_Surface* surface = ENG_TTF_RenderText_Blended(textFont, wrapped[i].c_str(), 0, renderColor);
            if (!surface)
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UICutsceneTextWriter.OnRender: Failed RenderText_Blended: %s.", SDL_GetError());
                continue;
            }

            const int surfaceW = surface->w;
            const int surfaceH = surface->h;

            SDL_Texture* texture = ENG_SDL_CreateTextureFromSurface(renderer, surface);
            ENG_SDL_DestroySurface(surface);
            if (!texture)
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UICutsceneTextWriter.OnRender: Failed CreateTextureFromSurface: %s.", SDL_GetError());
                continue;
            }

            SDL_FRect dst {
                std::round(bounds.x + (bounds.w - surfaceW) / 2.0f),    // Horizontal placement.
                static_cast<float>(startY) + i * fontHeight,            // Vertical placement with spacing.
                static_cast<float>(surfaceW),
                static_cast<float>(surfaceH)
            };

            if (!SDL_SetTextureAlphaMod(texture, renderColor.a))
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UICutsceneTextWriter.OnRender: Failed SetTextureAlphaMod: %s.", SDL_GetError());
                ENG_SDL_DestroyTexture(texture);
                continue;
            }

            if (!SDL_RenderTexture(renderer, texture, nullptr, &dst))
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UICutsceneTextWriter.OnRender: Failed RenderTexture: %s.", SDL_GetError());
                ENG_SDL_DestroyTexture(texture);
                continue;
            }

            ENG_SDL_DestroyTexture(texture);
        }
    }
}
