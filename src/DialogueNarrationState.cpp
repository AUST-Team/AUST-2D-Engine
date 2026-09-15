#include <SDL3_ttf/SDL_ttf.h>

#include "DialogueNarrationState.h"

#include "UITextEffectSystem.h"
#include "Configuration.h"
#include "UIDialogue.h"
#include "Miscs.h"
#include "AppRenderSystem.h"
#include "InteractCommand.h"
#include "MoveCommand.h"
#include "MemoryTracker.h"
#include "TileRegistry.h"
#include "StringMiscs.h"
#include "Unused.h"

DialogueNarrationState::DialogueNarrationState(UIDialogue& dialogue, const std::vector<std::string>& lines, const std::function<void()>& onFinished) :
    DialogueState(onFinished),
    dialogue(dialogue),
    dialogueLines(lines) {}

void DialogueNarrationState::Enter(UIDialogue& dialogueRef)
{
    UNUSED(dialogueRef);

    DialogueArrow upArrow = dialogue.GetArrow(Direction::Up);
    DialogueArrow downArrow = dialogue.GetArrow(Direction::Down);
    upArrowIcon.SetTileIndex(upArrow.tileIndex);
    downArrowIcon.SetTileIndex(downArrow.tileIndex);
    upArrowIcon.SetTransparency(upArrow.transparency);
    downArrowIcon.SetTransparency(downArrow.transparency);
    upArrowIcon.SetBorderColor(dialogue.GetBorderColor());
	downArrowIcon.SetBorderColor(dialogue.GetBorderColor());

    const SDL_FRect& bounds = dialogue.GetBounds();
    const float iconSize = dialogue.GetIconSize();

    upArrowIcon.SetBounds({
        .x = bounds.x + bounds.w - iconSize - 4.0f,  // Right side with 4px margin
        .y = bounds.y + 4.0f,                        // Up with 4px margin.
        .w = iconSize,
        .h = iconSize
    });

    downArrowIcon.SetBounds({
        .x = bounds.x + bounds.w - iconSize - 4.0f,  // Right side with 4px margin.
        .y = bounds.y + bounds.h - iconSize - 4.0f,  // Bottom with 4px margin.
        .w = iconSize,
        .h = iconSize
    });

    currentLineIndex = 0;
    scrollOffset = 0;
    dialogue.GetLineLabelPool().Clear();
    StartCurrentLine();
}

void DialogueNarrationState::HandleEvent(const SDL_Event& e)
{
    if (e.type == SDL_EVENT_MOUSE_WHEEL)
    {
        if (e.wheel.y > 0)
        {
            ScrollUp();
        }
        else if (e.wheel.y < 0)
        {
            ScrollDown();
        }
    }
}

void DialogueNarrationState::Update(double deltaTime)
{
    UITextEffectSystem& effectSystem = UITextEffectSystem::GetInstance();

    effectSystem.Update(deltaTime);

    if (effectSystem.HasEffect(effectID))
    {
        const std::string& currentText = effectSystem.GetText(effectID);
        wrappedLines = StringMiscs::WrapText(currentText, dialogue.GetBounds(), dialogue.GetTextFont(), dialogue.GetPadding().x);

        RecalculateMaxVisibleLines(dialogue.GetPadding().x);
        const int totalLines = static_cast<int>(wrappedLines.size());

        if (!effectSystem.IsFinished(effectID))
        {
            if (totalLines > maxVisibleLines)
            {
                scrollOffset = totalLines - maxVisibleLines;
            }
        }
        else
        {
            scrollOffset = std::max(0, totalLines - maxVisibleLines);
            UpdateLineLabelPool(); // Creates the labels for the line.
        }
    }
}

void DialogueNarrationState::Render(SDL_Renderer* renderer) const
{
    const UITextEffectSystem& effectSystem = UITextEffectSystem::GetInstance();

    const SDL_FRect& bounds = dialogue.GetBounds();
    TTF_Font* font = dialogue.GetTextFont();
    const SDL_Color& textColor = dialogue.GetTextColor();
    const UITextAlignment& alignment = dialogue.GetTextAlignment();
    const float lineSpacing = dialogue.GetLineSpacing();
    const SDL_FPoint& padding = dialogue.GetPadding();
    const float spacing = dialogue.GetCurrentFontHeight() * lineSpacing;
    const float effectiveScroll = static_cast<float>(scrollOffset) * spacing;
    const size_t fontHeight = dialogue.GetCurrentFontHeight();

    SDL_Rect clipRect {
        static_cast<int>(bounds.x),
        static_cast<int>(bounds.y),
        static_cast<int>(bounds.w),
        static_cast<int>(bounds.h)
    };

    if (!SDL_SetRenderClipRect(renderer, &clipRect))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "DialogueNarrationState.Render: SetRenderClipRect failed: %s", SDL_GetError());
        return;
    }

    if (effectSystem.HasEffect(effectID) && !effectSystem.IsFinished(effectID))
    {
        // Raw render during typing.
        const std::string currentText = effectSystem.GetText(effectID);
        std::vector<std::string> rawLines = StringMiscs::WrapText(currentText, bounds, font, padding.x);

        float yOffset = padding.y - effectiveScroll;

        for (const std::string& line : rawLines)
        {
            SDL_Surface* surface = ENG_TTF_RenderText_Blended(font, line.c_str(), 0, textColor);
            if (!surface)
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueNarrationState.Render: RenderText_Blended failed: %s", SDL_GetError());
                continue;
            }

            float textW = static_cast<float>(surface->w);
            float textH = static_cast<float>(surface->h);

            SDL_Texture* texture = ENG_SDL_CreateTextureFromSurface(renderer, surface);
            ENG_SDL_DestroySurface(surface);

            if (!texture)
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueNarrationState.Render: CreateTextureFromSurface failed: %s", SDL_GetError());
                continue;
            }

            float totalHeight = rawLines.size() * static_cast<float>(fontHeight);
            SDL_FPoint pos = GetAlignedPosition(bounds, textW, textH, alignment, totalHeight, padding.x);

            SDL_FRect dst {
                pos.x,
                bounds.y + yOffset,
                textW,
                textH
            };

            if (!SDL_RenderTexture(renderer, texture, nullptr, &dst))
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueNarrationState.Render: RenderTexture failed: %s", SDL_GetError());
            }

            ENG_SDL_DestroyTexture(texture);

            yOffset += spacing;
        }
    }
    else
    {
        // Finished typing, render UILabels.
        dialogue.GetLineLabelPool().Render(renderer, -effectiveScroll);
    }

    bool showUp = scrollOffset > 0;
    bool showDown = (wrappedLines.size() > (static_cast<size_t> (scrollOffset + maxVisibleLines) + 1));

    if (dialogue.ShouldShowArrows())
    {
        if (showUp)
        {
            upArrowIcon.Render(renderer);
        }

        if (showDown)
        {
            downArrowIcon.Render(renderer);
        }
    }

    if (!SDL_SetRenderClipRect(renderer, nullptr))
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "DialogueNarrationState.Render: SetRenderClipRect failed: %s", SDL_GetError());
    }
}

void DialogueNarrationState::HandleCommand(const Command* command)
{
    if (const auto* move = dynamic_cast<const MoveCommand*>(command))
    {
        switch (move->GetDirection())
        {
            case Direction::Up:
            {
                ScrollUp();
                break;
            }

            case Direction::Down:
            {
                ScrollDown();
                break;
            }

            default:
            {
                break;
            }
        }
    }
    else if (dynamic_cast<const InteractCommand*>(command))
    {
        UITextEffectSystem& effectSystem = UITextEffectSystem::GetInstance();
        if (effectSystem.HasEffect(effectID) && !effectSystem.IsFinished(effectID))
        {
            SkipTyping();
        }
        else
        {
            NextLine();
        }
    }
}

void DialogueNarrationState::Exit(UIDialogue& dialogueRef)
{ 
    UNUSED(dialogueRef);
    UITextEffectSystem::GetInstance().ForceFinish(effectID);
}

void DialogueNarrationState::RefreshState()
{
    UpdateLineLabelPool();
    UpdateScrollIcons();

    const SDL_FPoint& padding = dialogue.GetPadding();
    RecalculateMaxVisibleLines(padding.x);
    scrollOffset = std::max(0, (static_cast<int>(wrappedLines.size()) - maxVisibleLines));
}

bool DialogueNarrationState::IsShared() const { return false; }

void DialogueNarrationState::Resize(float widthScale, float heightScale)
{
	upArrowIcon.Resize(widthScale, heightScale);
	downArrowIcon.Resize(widthScale, heightScale);
}

void DialogueNarrationState::UpdateLineLabelPool()
{
    wrappedLines = StringMiscs::WrapText(dialogueLines[currentLineIndex], dialogue.GetBounds(), dialogue.GetTextFont());

    UILabelPool& pool = dialogue.GetLineLabelPool();
    pool.Resize(wrappedLines.size());
    pool.SetTextFont(dialogue.GetTextFont());
    pool.SetTextColor(dialogue.GetTextColor());
    pool.SetTextAlignment(dialogue.GetTextAlignment());
    pool.SetBorderColor(dialogue.GetBorderColor());

    const float lineSpacing = dialogue.GetLineSpacing();
    const SDL_FPoint& padding = dialogue.GetPadding();
    const SDL_FRect& bounds = dialogue.GetBounds();
    const size_t lineHeight = dialogue.GetCurrentFontHeight();
    const float spacing = lineHeight * lineSpacing;
    const float startY = bounds.y + padding.y;

    for (size_t i = 0; i < wrappedLines.size(); ++i)
    {
        UILabel& label = pool[i];
        SDL_FRect labelBounds {
            bounds.x + padding.x,
            startY + i * spacing,
            bounds.w - 2 * padding.x,
            static_cast<float>(lineHeight)
        };

        label.SetBounds(labelBounds);
        label.SetText(wrappedLines[i]);
        label.InvalidateCache();
    }
}

void DialogueNarrationState::UpdateScrollIcons()
{
    const SDL_FRect bounds = dialogue.GetBounds();

    const float iconDefaultSize = dialogue.GetIconSize();
    const size_t dialogueFontHeight = dialogue.GetCurrentFontHeight();

    const float fontScale = static_cast<float>(dialogue.GetOriginalFontHeight()) / dialogueFontHeight;
    const float iconSize = iconDefaultSize * fontScale;

    downArrowIcon.SetBounds({
        bounds.x + bounds.w - iconSize - 4.0f,  // right side with 4px margin
        bounds.y + bounds.h - iconSize - 4.0f,  // bottom with 4px margin
        iconSize,
        iconSize
    });

    upArrowIcon.SetBounds({
        bounds.x + bounds.w - iconSize - 4.0f,
        bounds.y + 8.0f,
        iconSize,
        iconSize
    });
}

void DialogueNarrationState::StartCurrentLine()
{
    UITextEffectSystem::GetInstance().AddEffect(effectID, dialogueLines[currentLineIndex], dialogue.GetTypewriterDelay());
    wrappedLines.clear();
    scrollOffset = 0;
    dialogue.GetLineLabelPool().Clear();
}

void DialogueNarrationState::ScrollUp() 
{ 
    if (scrollOffset > 0)
    {
        --scrollOffset;
    }
}

void DialogueNarrationState::ScrollDown()
{
    const int totalLines = static_cast<int>(wrappedLines.size());
    if (totalLines > maxVisibleLines)
    {
        const int maxScroll = totalLines - maxVisibleLines;
        if (scrollOffset < maxScroll)
        {
            ++scrollOffset;
        }
    }
}

void DialogueNarrationState::SkipTyping()
{
    UITextEffectSystem::GetInstance().ForceFinish(effectID);
    UpdateLineLabelPool();
}

void DialogueNarrationState::NextLine()
{
    currentLineIndex++;

    if (currentLineIndex >= dialogueLines.size())
    {
        if (onDialogueEndCallback)
        {
            onDialogueEndCallback();
        }
        dialogue.SetState(nullptr);
        return;
    }

    StartCurrentLine();
}

void DialogueNarrationState::RecalculateMaxVisibleLines(float padding)
{
    const size_t lineHeight = dialogue.GetCurrentFontHeight();
    const int availableHeight = static_cast<int>(dialogue.GetBounds().h - 2.0f * padding);
    maxVisibleLines = std::max(0, static_cast< int>(availableHeight / lineHeight) + 1);
}