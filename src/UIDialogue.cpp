#include "UIDialogue.h"

#include "DialogueState.h"
#include "Command.h"
#include "MemoryTracker.h"
#include "TileRegistry.h"
#include "FontManager.h"
#include "AppRenderSystem.h"

UIDialogue::UIDialogue(const std::string id, const SDL_FRect& bounds, const SDL_Color& backgroundColor, const SDL_Color& borderColor, TTF_Font* textFont, const SDL_Color& textColor, const UITextAlignment& textAlignment, const std::string& fontFamily, const std::array<DialogueArrow, enumCount<Direction>>& dialogueArrows, float blinkTime, const SDL_FPoint& padding, float lineSpacing, float iconSize) :
    UIElement(id, bounds, backgroundColor, borderColor),
    UITextStyleElement(textFont, textColor, textAlignment, fontFamily),
    dialogueArrows(dialogueArrows),
    timeBetweenBlinks(blinkTime),
    padding(padding),
    lineSpacing(lineSpacing),
    iconSize(iconSize) {}

UIDialogue::UIDialogue(UIDialogue&& other) noexcept :
    currentState(std::exchange(other.currentState, nullptr)),
    lineLabels(std::move(other.lineLabels)),
    onDialogueEndCallback(std::move(other.onDialogueEndCallback)),
    pendingState(std::exchange(other.pendingState, nullptr)),
    dialogueArrows(std::move(other.dialogueArrows)),
    timeBetweenBlinks(other.timeBetweenBlinks),
    blinkTimer(other.blinkTimer),
    showArrowsFlag(other.showArrowsFlag),
    padding(other.padding),
    lineSpacing(other.lineSpacing),
    iconSize(other.iconSize) {}

UIDialogue::~UIDialogue()
{
    if (currentState && !currentState->IsShared())
    {
        ENG_DELETE(currentState);
    }
}

void UIDialogue::HandleEvent(const SDL_Event& e)
{
    if (currentState)
    {
        currentState->HandleEvent(e);
    }
}

void UIDialogue::Update(double deltaTime) 
{ 
    blinkTimer += static_cast<float>(deltaTime);
    if (blinkTimer >= timeBetweenBlinks)
    {
        blinkTimer -= timeBetweenBlinks;
        showArrowsFlag = !showArrowsFlag;
    }

    if (currentState)
    {
        currentState->Update(deltaTime);
    }

    if (pendingState)
    {
        SetState(pendingState);
        pendingState = nullptr;
    }
}

void UIDialogue::HandleCommand(const Command* command)
{
    if (currentState)
    {
        currentState->HandleCommand(command);
    }
}

void UIDialogue::Resize(float widthScale, float heightScale)
{
    UIElement::Resize(widthScale, heightScale);

    float fontScale = std::min(widthScale, heightScale);
    float newFontSize = originalFontSize * fontScale;

    TTF_Font* dialogueFont = FontManager::GetInstance().GetFontByFamily(fontFamily, newFontSize);
    if (!dialogueFont)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIDialogue.Resize: Failed to load font: %s", SDL_GetError());
        return;
    }

    SetTextFont(dialogueFont);

    if (currentState)
    {
        currentState->RefreshState();
    }
}

void UIDialogue::ScheduleStateChange(DialogueState* nextState) { pendingState = nextState; }

UILabelPool& UIDialogue::GetLineLabelPool() { return lineLabels; }

void UIDialogue::SetOnDialogueEnd(const std::function<void()>& callback) 
{
    onDialogueEndCallback = callback;

    if (currentState)
    {
        currentState->SetOnDialogueEnd(onDialogueEndCallback);
        onDialogueEndCallback = nullptr;
    }
}

void UIDialogue::SetBounds(const SDL_FRect& newBounds)
{
    UIElement::SetBounds(newBounds);
    RefreshCurrentState();
}

void UIDialogue::SetBackgroundColor(const SDL_Color& newColor)
{
    UIElement::SetBackgroundColor(newColor);
    RefreshCurrentState();
}

void UIDialogue::SetTextColor(const SDL_Color& newColor)
{
    UITextStyleElement::SetTextColor(newColor);
    RefreshCurrentState();
}

void UIDialogue::SetTextFont(TTF_Font* newFont)
{
    UITextStyleElement::SetTextFont(newFont);
    RefreshCurrentState();
}

void UIDialogue::SetTextAlignment(const UITextAlignment& newAlignment)
{
    UITextStyleElement::SetTextAlignment(newAlignment);
    RefreshCurrentState();
}

void UIDialogue::SetLineSpacing(float newSpacing) 
{ 
    lineSpacing = newSpacing; 
    RefreshCurrentState();
}

float UIDialogue::GetLineSpacing() const { return lineSpacing; }

void UIDialogue::SetTypewriterDelay(float newDelay) { typewriterDelay = newDelay; }

float UIDialogue::GetTypewriterDelay() const { return typewriterDelay; }

void UIDialogue::SetPadding(const SDL_FPoint& newPadding)
{
    padding = newPadding;
    RefreshCurrentState();
}

SDL_FPoint UIDialogue::GetPadding() const { return padding; }

void UIDialogue::SetIconSize(float newSize)
{
    iconSize = newSize;
    RefreshCurrentState();
}

float UIDialogue::GetIconSize() const { return iconSize; }

void UIDialogue::SetArrowTileIndex(const std::string& tileName, Direction arrowDirection) { SetArrowTileIndex(TileRegistry::GetTileIndexByName(tileName), arrowDirection); }

void UIDialogue::SetArrowTileIndex(int tileIndex, Direction arrowDirection)
{
    dialogueArrows[ToIndex(arrowDirection)].tileIndex = tileIndex;
    RefreshCurrentState();
}

void UIDialogue::SetArrowTransparency(uint8_t alpha, Direction arrowDirection)
{
    dialogueArrows[ToIndex(arrowDirection)].transparency = alpha;
    RefreshCurrentState();
}

void UIDialogue::SetArrow(DialogueArrow arrow, Direction arrowDirection)
{
    dialogueArrows[ToIndex(arrowDirection)] = arrow;
    RefreshCurrentState();
}

DialogueArrow UIDialogue::GetArrow(Direction arrowDirection) const { return dialogueArrows[ToIndex(arrowDirection)]; }

ArrowVisibility UIDialogue::ShouldShowArrows() const { return showArrowsFlag; }

bool UIDialogue::HasPendingState() const { return pendingState != nullptr; }

bool UIDialogue::HasActiveState() const { return currentState != nullptr; }

UIDialogue& UIDialogue::operator=(UIDialogue&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    if (currentState && !currentState->IsShared())
    {
        ENG_DELETE(currentState);
        currentState = nullptr;
    }

    currentState = std::exchange(other.currentState, nullptr);
    lineLabels = std::move(other.lineLabels);
    onDialogueEndCallback = std::move(other.onDialogueEndCallback);
    pendingState = std::exchange(other.pendingState, nullptr);
    dialogueArrows = std::move(other.dialogueArrows);
    blinkTimer = other.blinkTimer;
    timeBetweenBlinks = other.timeBetweenBlinks;
    showArrowsFlag = other.showArrowsFlag;
    padding = other.padding;
    lineSpacing = other.lineSpacing;
    iconSize = iconSize;

    return *this;
}

void UIDialogue::SetState(DialogueState* newState)
{
    if (currentState)
    {
        currentState->Exit(*this);

        if (!currentState->IsShared())
        {
            ENG_DELETE(currentState);
        }

        currentState = nullptr;
    }

    currentState = newState;

    if (currentState)
    {
        // Set any cached callbacks.
        if (onDialogueEndCallback)
        {
            currentState->SetOnDialogueEnd(onDialogueEndCallback);
            onDialogueEndCallback = nullptr;
        }
        currentState->Enter(*this);
    }
}

void UIDialogue::RefreshCurrentState()
{
    if (currentState)
    {
        currentState->RefreshState();
    }
}

void UIDialogue::OnRender(SDL_Renderer* renderer) const
{
    if (currentState)
    {
        currentState->Render(renderer);
    }
}
