#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cmath>

#include "UITextField.h"

#include "AppRenderSystem.h"
#include "MemoryTracker.h"
#include "DataProvider.h"
#include "FontManager.h"

UITextField::UITextField(const std::string id, const SDL_FRect& bounds, const SDL_Color& backgroundColor, const SDL_Color& borderColor, const std::string& text, TTF_Font* textFont, const SDL_Color& textColor, const UITextAlignment& textAlignment, const std::string& fontFamily, const std::string& placeholderText, size_t maxLength, float blinkTime, bool numericOnly, bool showSlider, float textPadding, SDL_Color cursorColor, std::array<std::vector<std::function<void()>>, enumCount<TextFieldCallbackType>> callbacksByType, SDL_Color sliderTrackColor, SDL_Color sliderActiveColor, SDL_Color handleColor, float sliderPadding, FSize sliderSize, FSize handleSize) :
    UIElement(id, bounds, backgroundColor, borderColor),
    UITextElement(text, textFont, textColor, textAlignment, fontFamily),
    placeholder(placeholderText),
    maxLength(maxLength),
    callbacks(callbacksByType),
    timeForBlink(blinkTime),
    numericOnlyFlag(numericOnly),
    showSliderFlag(showSlider),
    textPadding(textPadding),
    sliderTrackColor(sliderTrackColor),
    sliderActiveColor(sliderActiveColor),
    handleColor(handleColor),
    sliderPadding(sliderPadding),
    sliderSize(sliderSize),
    handleSize(handleSize),
    cursorColor(cursorColor),
    originalHandleSize(handleSize),
    originalSliderSize(sliderSize) {}

void UITextField::HandleEvent(const SDL_Event& e)
{
    int currentVal = text.empty() ? 0 : std::clamp(std::stoi(text), 0, 100);
    float progress = static_cast<float>(currentVal) / 100.0f;

    SliderLayout layout = GetSliderLayout(progress);

    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        const SDL_FPoint point = { e.button.x, e.button.y };

        const SDL_FRect clickBounds = (numericOnlyFlag && showSliderFlag) ? layout.fullBounds : bounds;

        if (SDL_PointInRectFloat(&point, &clickBounds))
        {
            if (!focusedFlag)
            {
                focusedFlag = true;
                StartTextInput();
            }
        }
        else if (focusedFlag)
        {
            focusedFlag = false;
            StopTextInput();
        }
    }

    if (numericOnlyFlag && showSliderFlag && (e.type == SDL_EVENT_MOUSE_MOTION || e.type == SDL_EVENT_MOUSE_BUTTON_DOWN))
    {
        if (e.motion.state & SDL_BUTTON_LMASK)
        {
            const float mouseX = (e.type == SDL_EVENT_MOUSE_MOTION) ? e.motion.x : e.button.x;
            const float mouseY = (e.type == SDL_EVENT_MOUSE_MOTION) ? e.motion.y : e.button.y;
            const SDL_FPoint point = { mouseX, mouseY };

            if (point.x >= layout.track.x && point.x <= (layout.track.x + layout.track.w))
            {
                const float normalized = (point.x - layout.track.x) / layout.track.w;
                const int volumeVal = static_cast<int>(std::clamp(normalized, 0.0f, 1.0f) * 100.0f);

                text = std::to_string(volumeVal);
                TriggerCallbacks(TextFieldCallbackType::TextInputStop);
            }
        }
    }

    if (!focusedFlag)
    {
        return;
    }

    if (e.type == SDL_EVENT_TEXT_INPUT)
    {
        for (char c : std::string(e.text.text))
        {
            if (text.length() >= maxLength)
            {
                break;
            }

            if (numericOnlyFlag)
            {
                if (std::isdigit(static_cast<unsigned char>(c)))
                {
                    text += c;
                }
            }
            else
            {
                text += c;
            }
        }
    }
    else if (e.type == SDL_EVENT_KEY_DOWN)
    {
        if (e.key.key == SDLK_BACKSPACE && !text.empty())
        {
            text.pop_back();
        }
        else if (e.key.key == SDLK_RETURN || e.key.key == SDLK_KP_ENTER)
        {
            if (numericOnlyFlag && !text.empty())
            {
                const int val = std::clamp(std::stoi(text), 0, 100);
                text = std::to_string(val);
            }

            focusedFlag = false;
            StopTextInput();
        }
    }
}

void UITextField::Update(double deltaTime)
{
    if (focusedFlag) 
    {
        blinkTimer += static_cast<float>(deltaTime);
        if (blinkTimer >= timeForBlink)
        {
            showCursorFlag = !showCursorFlag;
            blinkTimer = 0.0f;
        }
    }
    else 
    {
        showCursorFlag = false;
        blinkTimer = 0.0f;
    }
}

void UITextField::Resize(float widthScale, float heightScale)
{
    UIElement::Resize(widthScale, heightScale);

    float fontScale = std::min(widthScale, heightScale);
    float newFontSize = originalFontSize * fontScale;

    TTF_Font* newFont = FontManager::GetInstance().GetFontByFamily(fontFamily, newFontSize);

    if (!newFont)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITextField.Resize: Failed to load new font for %s: %s", text.c_str(), SDL_GetError());
        return;
    }

    SetTextFont(newFont);

    sliderSize = originalSliderSize * fontScale;
    handleSize = originalHandleSize * fontScale;
}

void UITextField::AddCallback(const std::function<void()> callback, TextFieldCallbackType type)
{ 
    if (!callback)
    {
        return;
    }

    callbacks[ToIndex(type)].push_back(callback);
}

void UITextField::SetPlaceholderText(const std::string& newPlaceholder) { placeholder = newPlaceholder; }

std::string UITextField::GetPlaceholderText() const { return placeholder; }

void UITextField::SetMaxTextLength(size_t newLength) { maxLength = newLength; }

size_t UITextField::GetMaxTextLength() const { return maxLength; }

void UITextField::SetNumericOnlyFlag(bool numericOnly) { numericOnlyFlag = numericOnly; }

bool UITextField::GetNumericOnlyFlag() const { return numericOnlyFlag; }

void UITextField::SetShowSliderFlag(bool showSlider) { showSliderFlag = showSlider; }

bool UITextField::GetShowSliderFlag() const { return showSliderFlag; }

int UITextField::GetTextAsInteger() const
{
    if (numericOnlyFlag && !text.empty())
    {
        try
        {

            return std::stoi(text);
        }
        catch (const std::exception&)
        {
            // I HATE EXCEPTIONS!!!!!!!!!!!!!!!!!!!!!!!!!!!
            return -1;
        }
    }

    return -1;
}

void UITextField::SetTextPadding(float newPadding) { textPadding = newPadding; }

float UITextField::GetTextPadding() const { return textPadding; }

void UITextField::SetCursorColor(const SDL_Color& newColor) { cursorColor = newColor; }

SDL_Color UITextField::GetCursorColor() const { return cursorColor; }

void UITextField::SetSliderTrackColor(const SDL_Color& newColor) { sliderTrackColor = newColor; }

SDL_Color UITextField::GetSliderTrackColor() const { return sliderTrackColor; }

void UITextField::SetSliderActiveColor(const SDL_Color& newColor) { sliderActiveColor = newColor; }

SDL_Color UITextField::GetSliderActiveColor() const { return sliderActiveColor; }

void UITextField::SetHandleColor(const SDL_Color& newColor) { handleColor = newColor; }

SDL_Color UITextField::GetHandleColor() const { return handleColor; }

void UITextField::SetSliderPadding(float newPadding) { sliderPadding = newPadding; }

float UITextField::GetSliderPadding() const { return sliderPadding; }

void UITextField::SetSliderSize(const FSize& newSize) { sliderSize = newSize; }

void UITextField::OverrideSliderSize(const FSize& newSize)
{
    SetSliderSize(newSize);
    originalSliderSize = newSize;
}

FSize UITextField::GetSliderSize() const { return sliderSize; }

void UITextField::SetHandleSize(const FSize& newSize) { handleSize = newSize; }

void UITextField::OverrideHandleSize(const FSize& newSize)
{
    SetHandleSize(newSize);
    originalHandleSize = newSize;
}

FSize UITextField::GetHandleSize() const { return handleSize; }

void UITextField::StartTextInput()
{
    SDL_Window* window = AppRenderSystem::GetInstance().GetWindow();
	if (!SDL_TextInputActive(window))
	{
		SDL_StartTextInput(window);

        TriggerCallbacks(TextFieldCallbackType::TextInputStart);
	}
}

void UITextField::StopTextInput()
{
	SDL_Window* window = AppRenderSystem::GetInstance().GetWindow();
	if (SDL_TextInputActive(window))
	{
		SDL_StopTextInput(window);

        TriggerCallbacks(TextFieldCallbackType::TextInputStop);
	}
}

void UITextField::TriggerCallbacks(TextFieldCallbackType type)
{
    for (const std::function<void()>& func : callbacks[ToIndex(type)])
    {
        if (func)
        {
            func();
        }
    }
}

SliderLayout UITextField::GetSliderLayout(float progress) const
{
    SliderLayout layout{};

    layout.track = SDL_FRect {
        bounds.x + bounds.w + sliderPadding,
        bounds.y + (bounds.h * 0.5f) - (sliderSize.h * 0.5f),
        sliderSize.w,
        sliderSize.h
    };

    layout.activeTrack = layout.track;
    layout.activeTrack.w = layout.track.w * progress;

    const float handleX = layout.track.x + (layout.track.w * progress) - (handleSize.w * 0.5f);
    const float handleY = bounds.y + (bounds.h * 0.5f) - (handleSize.h * 0.5f);

    layout.handle = SDL_FRect { 
        handleX, 
        handleY, 
        handleSize.w,
        handleSize.h
    };

    const float totalWidth = bounds.w + sliderPadding + sliderSize.w + (handleSize.w * 0.5f);
    layout.fullBounds = SDL_FRect{ bounds.x, bounds.y, totalWidth, bounds.h };

    return layout;
}

void UITextField::OnRender(SDL_Renderer* renderer) const
{
    std::string displayText = text.empty() && !focusedFlag ? placeholder : text;
    SDL_Color colorToUse = text.empty() && !focusedFlag ? Configuration::Get().ui.disabled : textColor;

    SDL_Rect clipRect {
        static_cast<int>(bounds.x),
        static_cast<int>(bounds.y),
        static_cast<int>(bounds.w),
        static_cast<int>(bounds.h)
    };

    if (!SDL_SetRenderClipRect(renderer, &clipRect))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITextField.OnRender: Failed SetRenderClipRect: %s", SDL_GetError());
        return;
    }

    do
    {
        if (displayText.empty())
        {
            break;
        }

        SDL_Surface* surface = ENG_TTF_RenderText_Blended(textFont, displayText.c_str(), 0, colorToUse);
        if (!surface)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITextField.OnRender: Failed RenderText_Blended: %s", SDL_GetError());
            break;
        }

        const float textW = static_cast<float>(surface->w);
        const float textH = static_cast<float>(surface->h);

        SDL_Texture* texture = ENG_SDL_CreateTextureFromSurface(renderer, surface);
        ENG_SDL_DestroySurface(surface);
        if (!texture)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITextField.OnRender: Failed CreateTextureFromSurface: %s", SDL_GetError());
            break;
        }

        float maxVisible = bounds.w - 2.0f * textPadding;

        if (textW > maxVisible)
        {
            scrollOffset = textW - maxVisible;
        }
        else
        {
            scrollOffset = 0.0f;
        }

        SDL_FPoint pos = GetAlignedPosition(bounds, textW, textH, textAlignment);

        SDL_FRect dst {
            bounds.x + textPadding - scrollOffset, // Left padding, scrolled.
            bounds.y + (bounds.h - textH) / 2.0f,
            textW,
            textH
        };

        if (!SDL_RenderTexture(renderer, texture, nullptr, &dst))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UILabel.OnRender: Failed RenderTexture for %s: %s", text.c_str(), SDL_GetError());
            ENG_SDL_DestroyTexture(texture);
            break;
        }

        ENG_SDL_DestroyTexture(texture);

        if (focusedFlag && showCursorFlag)
        {
            SDL_FRect cursor {
                bounds.x + textW - scrollOffset + textPadding,
                bounds.y + (bounds.h - textH) / 2.0f,
                2.0f,
                textH
            };

            if (!SDL_SetRenderDrawColor(renderer, cursorColor.r, cursorColor.g, cursorColor.b, cursorColor.a))
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITextField.OnRender: Failed SetRenderDrawColor: %s", SDL_GetError());
                break;
            }

            if (!SDL_RenderFillRect(renderer, &cursor))
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITextField.OnRender: Failed RenderFillRect: %s", SDL_GetError());
                break;
            }
        }
    } while (false);

    // What if you wanted to use go-to but C++ said: jump skips initialisation of X variable.
    // Ok, C++, can you see that I won't use the variables at any point of where the tag is (here by-the-way).

    if (!SDL_SetRenderClipRect(renderer, nullptr))
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "UITextField.OnRender: Failed SetRenderClipRect: %s", SDL_GetError());
        return;
    }

    if (numericOnlyFlag && showSliderFlag)
    {
        int currentVal = text.empty() ? 0 : std::clamp(std::stoi(text), 0, 100);
        float progress = static_cast<float>(currentVal) / 100.0f;

        SliderLayout layout = GetSliderLayout(progress);


        if (!SDL_SetRenderDrawColor(renderer, sliderTrackColor.r, sliderTrackColor.g, sliderTrackColor.b, sliderTrackColor.a))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITextField.OnRender: Failed SetRenderDrawColor: %s", SDL_GetError());
            return;
        }

        if (!SDL_RenderFillRect(renderer, &layout.track))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITextField.OnRender: Failed RenderFillRect: %s", SDL_GetError());
            return;
        }

        if (!SDL_SetRenderDrawColor(renderer, sliderActiveColor.r, sliderActiveColor.g, sliderActiveColor.b, sliderActiveColor.a))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITextField.OnRender: Failed SetRenderDrawColor: %s", SDL_GetError());
            return;
        }

        if (!SDL_RenderFillRect(renderer, &layout.activeTrack))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITextField.OnRender: Failed RenderFillRect: %s", SDL_GetError());
            return;
        }

        if (!SDL_SetRenderDrawColor(renderer, handleColor.r, handleColor.g, handleColor.b, handleColor.a))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITextField.OnRender: Failed SetRenderDrawColor: %s", SDL_GetError());
            return;
        }

        if (!SDL_RenderFillRect(renderer, &layout.handle))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITextField.OnRender: Failed RenderFillRect: %s", SDL_GetError());
            return;
        }
    }
}

