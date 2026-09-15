#include <SDL3_ttf/SDL_ttf.h>

#include "DialogueChoiceState.h"

#include "UITextEffectSystem.h"
#include "Configuration.h"
#include "UIDialogue.h"
#include "Miscs.h"
#include "MoveCommand.h"
#include "InteractCommand.h"
#include "TileRegistry.h"
#include "MemoryTracker.h"
#include "StringMiscs.h"
#include "Unused.h"

DialogueChoiceState::DialogueChoiceState(UIDialogue& dialogue, const std::string& question, const std::vector<std::string>& choices, const std::vector<std::function<void()>>& callbacks, const std::function<void()>& onDialogueEnd) :
    DialogueState(onDialogueEnd),
    dialogue(dialogue),
    question(question),
    choices(choices),
    choiceCallbacks(callbacks)
{
    if (choices.size() != choiceCallbacks.size())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueChoice: Number of choices and callbacks do not match!");
    }
}

void DialogueChoiceState::Enter(UIDialogue& dialogueRef)
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
        bounds.x + bounds.w - iconSize - 4.0f,  // Right side with 4px margin
        bounds.y + 4.0f,                        // Up with 4px margin.
        iconSize,
        iconSize
    });

    downArrowIcon.SetBounds({
        bounds.x + bounds.w - iconSize - 4.0f,  // Right side with 4px margin.
        bounds.y + bounds.h - iconSize - 4.0f,  // Bottom with 4px margin.
        iconSize,
        iconSize
    });

    selectedIndex = 0;
    scrollOffset = 0;
    choicesTypingFlag = true;

    UITextEffectSystem& effectSystem = UITextEffectSystem::GetInstance();

    effectSystem.AddEffect(questionEffectID, question, dialogue.GetTypewriterDelay());

    for (size_t i = 0; i < choices.size(); ++i)
    {
        std::string id = choiceEffectIDPrefix + std::to_string(i);
        effectSystem.AddEffect(id, choices[i], 30);
    }

    dialogue.GetLineLabelPool().Clear();
    choiceLabelPools.resize(choices.size());
    wrappedChoiceLines.resize(choices.size());
}

void DialogueChoiceState::HandleEvent(const SDL_Event& e)
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

void DialogueChoiceState::Update(double deltaTime)
{
    UITextEffectSystem& effectSystem = UITextEffectSystem::GetInstance();

    effectSystem.Update(deltaTime);

    questionTypingFlag = false;
    if (effectSystem.HasEffect(questionEffectID))
    {
        const std::string currentText = effectSystem.GetText(questionEffectID);
        wrappedQuestionLines = StringMiscs::WrapText(currentText, dialogue.GetBounds(), dialogue.GetTextFont());
        UpdateChoiceLabelPositions();   // Update the choice position while typewriting.

        RecalculateMaxVisibleLines(dialogue.GetPadding().x);
        const int totalLines = static_cast<int>(wrappedQuestionLines.size());

        if (!effectSystem.IsFinished(questionEffectID))
        {
            questionTypingFlag = true;
            if (totalLines > maxVisibleLines)
            {
                scrollOffset = totalLines - maxVisibleLines;
            }
        }
        else
        {
            UpdateQuestionLabelPool(); // Make the question label pool.
        }
    }

    // Check if choices finished typing.
    choicesTypingFlag = false;
    for (size_t i = 0; i < choices.size(); ++i)
    {
        const std::string id = choiceEffectIDPrefix + std::to_string(i);
        if (effectSystem.HasEffect(id))
        {
            if (!effectSystem.IsFinished(id))
            {
                choicesTypingFlag = true;
                UpdateChoiceLabelPositions();
            }
            else
            {
                AddChoiceToPool(effectSystem.GetText(id), i);   // Once finished, add the choice to the label pool.
            }
        }
    }

    if (!finishTypingFlag && !choicesTypingFlag && !questionTypingFlag)
    {
        finishTypingFlag = true;
        UpdateChoiceHighlighting();
    }
}

void DialogueChoiceState::Render(SDL_Renderer* renderer) const
{
    UITextEffectSystem& effectSystem = UITextEffectSystem::GetInstance();

    const SDL_FRect& bounds = dialogue.GetBounds();
    const SDL_Color& textColor = dialogue.GetTextColor();
    TTF_Font* font = dialogue.GetTextFont();
    const UITextAlignment& alignment = dialogue.GetTextAlignment();
    float lineSpacing = dialogue.GetLineSpacing();
    const SDL_FPoint& padding = dialogue.GetPadding();
    float spacing = static_cast<float>(TTF_GetFontHeight(font)) * lineSpacing;
    float effectiveScroll = static_cast<float>(scrollOffset) * spacing;

    SDL_Rect clipRect {
        static_cast<int>(bounds.x),
        static_cast<int>(bounds.y),
        static_cast<int>(bounds.w),
        static_cast<int>(bounds.h)
    };

    if (!SDL_SetRenderClipRect(renderer, &clipRect))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "DialogueChoiceState.Render: SetRenderClipRect failed: %s", SDL_GetError());
        return;
    }

    float yOffset = padding.y - effectiveScroll;


    if (effectSystem.HasEffect(questionEffectID) && !effectSystem.IsFinished(questionEffectID))
    {
        // If not finished typing, raw-render the text.
        const std::string currentText = effectSystem.GetText(questionEffectID);
        wrappedQuestionLines = StringMiscs::WrapText(currentText, bounds, font, padding.x);

        for (const std::string& line : wrappedQuestionLines)
        {
            SDL_Surface* surface = ENG_TTF_RenderText_Blended(font, line.c_str(), 0, textColor);
            if (!surface)
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueChoiceState.Render: RenderText_Blended failed: %s", SDL_GetError());
                continue;
            }

            float textW = static_cast<float>(surface->w);
            float textH = static_cast<float>(surface->h);

            SDL_Texture* texture = ENG_SDL_CreateTextureFromSurface(renderer, surface);
            ENG_SDL_DestroySurface(surface);

            if (!texture)
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueChoiceState.Render: CreateTextureFromSurface failed: %s", SDL_GetError());
                continue;
            }

            float totalHeight = wrappedQuestionLines.size() * textH;
            SDL_FPoint pos = GetAlignedPosition(bounds, textW, textH, alignment, totalHeight, padding.x);

            SDL_FRect dst {
                pos.x,
                bounds.y + yOffset,
                textW,
                textH
            };

            if (!SDL_RenderTexture(renderer, texture, nullptr, &dst))
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueChoiceState.Render: RenderTexture failed: %s", SDL_GetError());
            }

            ENG_SDL_DestroyTexture(texture);

            yOffset += spacing;
        }
    }
    else
    {
        // If finished typing, render the labels.
        dialogue.GetLineLabelPool().Render(renderer, -effectiveScroll);
        yOffset += wrappedQuestionLines.size() * spacing;
    }

    // Add one line worth of space between the question and choices.
    yOffset += spacing;

    for (size_t i = 0; i < choices.size(); ++i)
    {
        const std::string id = choiceEffectIDPrefix + std::to_string(i);
        if (effectSystem.HasEffect(id) && !effectSystem.IsFinished(id))
        {
            // If the choice is still typing, raw-render it.
            const std::string currentText = effectSystem.GetText(id);
            std::vector<std::string> rawLines = StringMiscs::WrapText(currentText, bounds, font, padding.x);

            for (const std::string& line : rawLines)
            {
                SDL_Surface* surface = ENG_TTF_RenderText_Blended(font, line.c_str(), 0, textColor);
                if (!surface)
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueChoiceState.Render: RenderText_Blended failed: %s", SDL_GetError());
                    continue;
                }

                float textW = static_cast<float>(surface->w);
                float textH = static_cast<float>(surface->h);

                SDL_Texture* texture = ENG_SDL_CreateTextureFromSurface(renderer, surface);
                ENG_SDL_DestroySurface(surface);

                if (!texture)
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueChoiceState.Render: CreateTextureFromSurface failed: %s", SDL_GetError());
                    continue;
                }

                float totalHeight = rawLines.size() * textH;
                SDL_FPoint pos = GetAlignedPosition(bounds, textW, textH, alignment, totalHeight, padding.x);

                SDL_FRect dst {
                    pos.x,
                    bounds.y + yOffset,
                    textW,
                    textH
                };

                if (!SDL_RenderTexture(renderer, texture, nullptr, &dst))
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueChoiceState.Render: RenderTexture failed: %s", SDL_GetError());
                }

                ENG_SDL_DestroyTexture(texture);

                yOffset += spacing;
            }
        }
        else
        {
            // If the choice finished, render it from the labels.
            choiceLabelPools[i].Render(renderer, -effectiveScroll);
            yOffset += wrappedChoiceLines[i].size() * spacing;
        }
    }

    bool showUp = scrollOffset > 0;
    size_t totalLines = wrappedQuestionLines.size() + 1; // + 1 from that space in between the question and choices.

    for (size_t i = 0; i < choices.size(); ++i)
    {
        totalLines += wrappedChoiceLines[i].size();
    }

    bool showDown = (totalLines > (static_cast<size_t> (scrollOffset + maxVisibleLines) + 1));

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
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "DialogueChoiceState.Render: SetRenderClipRect failed: %s", SDL_GetError());
    }
}

void DialogueChoiceState::HandleCommand(const Command* command)
{
    if (const auto* move = dynamic_cast<const MoveCommand*>(command))
    {
        switch (move->GetDirection())
        {
            case Direction::Up: 
            {
                SelectPreviousChoice();
                break;
            }

            case Direction::Down: 
            {
                SelectNextChoice();
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
        if (!finishTypingFlag)
        {
            SkipTyping();
        }
        else
        {
            ConfirmChoice();
        }
    }
}

void DialogueChoiceState::RefreshState()
{
    UpdateQuestionLabelPool();
    RecalculateMaxVisibleLines(dialogue.GetPadding().x);
    scrollOffset = std::max(0, (static_cast<int>(wrappedQuestionLines.size()) - static_cast<int>(maxVisibleLines)));
    UpdateChoiceLabelPool();
    UpdateScrollIcons();
}

bool DialogueChoiceState::IsShared() const { return false; }

void DialogueChoiceState::Exit(UIDialogue& dialogueRef) { DialogueState::Exit(dialogueRef); }

void DialogueChoiceState::Resize(float widthScale, float heightScale)
{
    upArrowIcon.Resize(widthScale, heightScale);
    downArrowIcon.Resize(widthScale, heightScale);
}

void DialogueChoiceState::UpdateQuestionLabelPool()
{
    TTF_Font* font = dialogue.GetTextFont();
    const SDL_Color& color = dialogue.GetTextColor();
    const UITextAlignment& align = dialogue.GetTextAlignment();
    const float lineSpacing = dialogue.GetLineSpacing();
    const SDL_FPoint& padding = dialogue.GetPadding();
    const SDL_FRect& bounds = dialogue.GetBounds();
    UILabelPool& questionLabels = dialogue.GetLineLabelPool();
    const size_t lineHeight = dialogue.GetCurrentFontHeight();

    wrappedQuestionLines = StringMiscs::WrapText(question, bounds, dialogue.GetTextFont());

    questionLabels.Resize(wrappedQuestionLines.size());
    questionLabels.SetTextFont(font);
    questionLabels.SetTextColor(color);
    questionLabels.SetTextAlignment(align);
	questionLabels.SetBorderColor(dialogue.GetBorderColor());

    const float y = bounds.y + padding.y;
    const float spacing = lineHeight * lineSpacing;

    for (size_t i = 0; i < wrappedQuestionLines.size(); ++i)
    {
        UILabel& label = questionLabels[i];
        label.SetText(wrappedQuestionLines[i]);
        label.SetBounds({
            bounds.x + padding.x,
            y + spacing * i,
            bounds.w - 2 * padding.x,
            static_cast<float>(lineHeight)
        });
        label.InvalidateCache();
    }
}

void DialogueChoiceState::UpdateChoiceLabelPool()
{
    TTF_Font* font = dialogue.GetTextFont();
    const SDL_Color& normalColor = dialogue.GetTextColor();
    const SDL_Color selectedColor = finishTypingFlag ? SDL_Color { 255, 255, 0, 255 } : normalColor; // { 255, 255, 0, 255 } = Yellow.
    const std::string prefix = finishTypingFlag ? " > " : "";
    const UITextAlignment& align = dialogue.GetTextAlignment();
    const float lineSpacing = dialogue.GetLineSpacing();
    const SDL_FPoint& padding = dialogue.GetPadding();
    const size_t lineHeight = dialogue.GetCurrentFontHeight();
    const SDL_FRect& bounds = dialogue.GetBounds();
    const float spacing = lineHeight * lineSpacing;

    wrappedChoiceLines.resize(choices.size());
    for (size_t i = 0; i < choices.size(); ++i)
    {
        wrappedChoiceLines[i] = StringMiscs::WrapText(choices[i], bounds, font);
    }

    // Resize the per-choice pools (1 for each choice).
    choiceLabelPools.resize(choices.size());

    // Starting height of the dialogue box + edge padding + 1 line worth of spacing + the question's size.
    float y = bounds.y + padding.x + spacing + wrappedQuestionLines.size() * spacing;

    for (size_t i = 0; i < choices.size(); ++i)
    {
        UILabelPool& pool = choiceLabelPools[i];
        pool.Resize(wrappedChoiceLines[i].size());
        pool.SetTextFont(font);
        pool.SetTextAlignment(align);
        pool.SetTextColor(i == selectedIndex ? selectedColor : normalColor);
		pool.SetBorderColor(dialogue.GetBorderColor());

        for (size_t j = 0; j < wrappedChoiceLines[i].size(); ++j)
        {
            UILabel& label = pool[j];
            label.SetText(i == selectedIndex && j == 0 ? prefix + wrappedChoiceLines[i][j] : wrappedChoiceLines[i][j]);
            label.SetBounds({
                bounds.x + padding.x,
                y,
                bounds.w - 2 * padding.x,
                static_cast<float>(lineHeight)
            });
            label.InvalidateCache();

            y += spacing;
        }
    }
}

void DialogueChoiceState::UpdateChoiceHighlighting()
{
    if (!finishTypingFlag)
    {
        return;
    }

    const SDL_Color& normalColor = dialogue.GetTextColor();
    const SDL_Color selectedColor { 255, 255, 0, 255 };

    for (size_t i = 0; i < choiceLabelPools.size(); ++i)
    {
        UILabelPool& pool = choiceLabelPools[i];
        for (size_t j = 0; j < pool.Size(); ++j)
        {
            UILabel& label = pool[j];
            label.SetText(i == selectedIndex && j == 0 ? " > " + wrappedChoiceLines[i][j] : wrappedChoiceLines[i][j]);
            label.SetTextColor(i == selectedIndex ? selectedColor : normalColor);
            label.InvalidateCache();
        }
    }
}

void DialogueChoiceState::UpdateChoiceLabelPositions()
{
    const float lineSpacing = dialogue.GetLineSpacing();
    const SDL_FPoint& padding = dialogue.GetPadding();
    TTF_Font* font = dialogue.GetTextFont(); // Reference to a constant pointer.
    const float spacing = dialogue.GetCurrentFontHeight() * lineSpacing;
    const SDL_FRect& bounds = dialogue.GetBounds();

    float y = bounds.y + padding.y + wrappedQuestionLines.size() * spacing + spacing;

    UITextEffectSystem& effectSystem = UITextEffectSystem::GetInstance();

    for (size_t i = 0; i < choiceLabelPools.size(); ++i)
    {
        UILabelPool& pool = choiceLabelPools[i];

        const std::string id = choiceEffectIDPrefix + std::to_string(i);

        if (effectSystem.HasEffect(id))
        {
            // Still typing, simulate wrap height from current text.
            const std::string text = effectSystem.GetText(id);
            const std::vector<std::string>& wrapped = StringMiscs::WrapText(text, bounds, font, padding.x);
            y += wrapped.size() * spacing;
            // Skip updating the bounds, not rendering via LabelPool yet.
            continue;
        }

        for (size_t j = 0; j < pool.Size(); ++j)
        {
            UILabel& label = pool[j];
            SDL_FRect labelBounds = label.GetBounds();
            labelBounds.y = y;   // Only need to update the Y.
            label.SetBounds(labelBounds);
            label.InvalidateCache();

            y += spacing;
        }
    }
}

void DialogueChoiceState::UpdateScrollIcons()
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

void DialogueChoiceState::AddChoiceToPool(std::string choice, size_t index)
{
    TTF_Font* font = dialogue.GetTextFont();
    const UITextAlignment& align = dialogue.GetTextAlignment();
    const SDL_Color& normalColor = dialogue.GetTextColor();
    // If still typing, set the prefix and selected color to 'default' (so they don't show).
    const SDL_Color selectedColor = finishTypingFlag ? SDL_Color { 255, 255, 0, 255 } : normalColor; // { 255, 255, 0, 255 } = Yellow.
    const std::string prefix = finishTypingFlag ? " > " : "";
    const float lineSpacing = dialogue.GetLineSpacing();
    const SDL_FPoint& padding = dialogue.GetPadding();
    const size_t lineHeight = dialogue.GetCurrentFontHeight();
    const float spacing = lineHeight * lineSpacing;
    const SDL_FRect& bounds = dialogue.GetBounds();

    wrappedChoiceLines[index] = StringMiscs::WrapText(choice, bounds, font, padding.x);

    UILabelPool& labelPool = choiceLabelPools[index];
    labelPool.Resize(wrappedChoiceLines[index].size());
    labelPool.SetTextFont(font);
    labelPool.SetTextAlignment(align);
    labelPool.SetTextColor(index == selectedIndex ? selectedColor : normalColor);
    labelPool.SetBorderColor(dialogue.GetBorderColor());

    // Calculate base Y offset: The top bound of the dialogue box + padding space + 1 line worth of padding (between question and choices) + the question's size;
    float y = bounds.y + padding.y + spacing + wrappedQuestionLines.size() * spacing;

    const UITextEffectSystem& effectSystem = UITextEffectSystem::GetInstance();

    for (size_t i = 0; i < index; ++i)
    {
        const std::string id = choiceEffectIDPrefix + std::to_string(i);

        if (effectSystem.HasEffect(id))
        {
            // Still typing, simulate wrap height from current text.
            const std::string text = effectSystem.GetText(id);
            const std::vector<std::string>& wrapped = StringMiscs::WrapText(text, bounds, font, padding.x);
            y += wrapped.size() * spacing;
        }
        else
        {
            // No longer typing, get the full text.
            y += wrappedChoiceLines[i].size() * spacing;
        }
    }

    for (size_t i = 0; i < wrappedChoiceLines[index].size(); ++i)
    {
        UILabel& label = labelPool[i];
        label.SetText(index == selectedIndex && i == 0 ? prefix + wrappedChoiceLines[index][i] : wrappedChoiceLines[index][i]);
        label.SetBounds({
            bounds.x + padding.x,
            y,
            bounds.w - 2 * padding.x,
            static_cast<float>(lineHeight)
        });
        label.InvalidateCache();

        y += spacing;
    }
}

void DialogueChoiceState::ScrollUp()
{
    if (scrollOffset > 0)
    {
        --scrollOffset;
    }
}

void DialogueChoiceState::ScrollDown()
{
    int totalLines = static_cast<int>(wrappedQuestionLines.size()) + 1;

    for (size_t i = 0; i < choices.size(); ++i)
    {
        totalLines += static_cast<int>(wrappedChoiceLines[i].size());
    }

    if (totalLines > maxVisibleLines)
    {
        const int maxScroll = totalLines - maxVisibleLines;
        if (scrollOffset < maxScroll)
        {
            ++scrollOffset;
        }
    }
}

void DialogueChoiceState::SelectPreviousChoice() 
{ 
    if (!finishTypingFlag)
    {
        return;
    }

    selectedIndex = static_cast<int>((selectedIndex + choices.size() - 1) % choices.size());
    UpdateChoiceHighlighting();
}

void DialogueChoiceState::SelectNextChoice() 
{ 
    if (!finishTypingFlag)
    {
        return;
    }

    selectedIndex = (static_cast<size_t>(selectedIndex) + 1) % choices.size(); 
    UpdateChoiceHighlighting();
}

void DialogueChoiceState::SkipTyping()
{
    finishTypingFlag = true;

    UITextEffectSystem& effectSystem = UITextEffectSystem::GetInstance();

    effectSystem.ForceFinish(questionEffectID);
    UpdateQuestionLabelPool();
    for (size_t i = 0; i < choices.size(); ++i)
    {
        effectSystem.ForceFinish(choiceEffectIDPrefix + std::to_string(i));
    }

    UpdateChoiceLabelPool();
}

void DialogueChoiceState::ConfirmChoice()
{
    if (selectedIndex < choiceCallbacks.size() && choiceCallbacks[selectedIndex])
    {
        choiceCallbacks[selectedIndex]();
    }

    dialogue.SetState(nullptr);
}

void DialogueChoiceState::RecalculateMaxVisibleLines(float padding)
{
    const int lineHeight = TTF_GetFontHeight(dialogue.GetTextFont());
    const int availableHeight = static_cast<int>(dialogue.GetBounds().h - 2 * padding);
    maxVisibleLines = std::max(0, static_cast<int>(availableHeight / lineHeight) + 1);
}
