#include "UISaveSlot.h"

#include "MemoryTracker.h"
#include "UILabel.h"
#include "UIButton.h"
#include "SaveManager.h"
#include "AppStateManager.h"
#include "GameTime.h"
#include "GameState.h"
#include "GuardManager.h"
#include "PlayerStrategyManager.h"
#include "Miscs.h"
#include "UIFactory.h"

UISaveSlot::UISaveSlot(const std::string id, const SDL_FRect& bounds, const SDL_Color& backgroundColor, const SDL_Color& borderColor, TTF_Font* textFont, const SDL_Color& textColor, const UITextAlignment& textAlignment, const std::string& fontFamily, int slotId, SaveSlotMode mode) :
    UIPanel(id, bounds, backgroundColor, borderColor),
    slotId(slotId),
    mode(mode)
{
    slotLabel = ENG_NEW(UILabel,
        "__" + id + "_" + "slot_label",
        SDL_FRect { bounds.x + 10, bounds.y + 10, 300, 90 },
        backgroundColor,
        borderColor,
        "",
        textFont,
        textColor,
        textAlignment,
        fontFamily
    );
    AddElement(slotLabel);

    saveLoadBtn = ENG_NEW(UIButton,
        "__" + id + "_" + "save_load_button",
        SDL_FRect { bounds.x + bounds.w - 220, bounds.y + 15, 100, 30 },
        backgroundColor,
        borderColor,
        mode == SaveSlotMode::Load ? "LOAD" : "SAVE",
        textFont,
        textColor,
        textAlignment,
        fontFamily
    );
    saveLoadBtn->AddCallback([this]() { OnMainAction(); }, ButtonCallbackType::Click);
    AddElement(saveLoadBtn);

    deleteBtn = ENG_NEW(UIButton,
        "__" + id + "_" + "delete_button",
        SDL_FRect { bounds.x + bounds.w - 110, bounds.y + 15, 100, 30 },
        backgroundColor,
        borderColor,
        "DELETE",
        textFont,
        textColor,
        textAlignment,
        fontFamily
    );

    deleteBtn->AddCallback([this]() {
        saveLoadBtn->SetVisible(false);
        deleteBtn->SetVisible(false);
        confirmPanel->SetVisible(true);
    }, ButtonCallbackType::Click);
    AddElement(deleteBtn);

    slotLabel->SetBorderVisible(HIDE_BORDERS);
    saveLoadBtn->SetBorderVisible(HIDE_BORDERS);
    deleteBtn->SetBorderVisible(HIDE_BORDERS);

    BuildConfirmPanel(textFont, textColor, textAlignment, fontFamily);

    Refresh();
}

void UISaveSlot::Update(double deltaTime)
{
    uint64_t currentRev = SaveManager::GetSlotRevision(slotId);

    if (lastKnownRevision != currentRev)
    {
        Refresh();
        lastKnownRevision = currentRev;
    }

    if (mode == SaveSlotMode::Save && saveLoadBtn && !confirmPanel->IsVisible())
    {
        bool canSave = (GuardManager::GetInstance().GetAlertedGuardCount() == 0) &&
            (PlayerStrategyManager::GetCurrentStrategyType() == PlayerStrategyType::Normal);

        saveLoadBtn->SetActive(canSave);
    }

    UIPanel::Update(deltaTime);
}

void UISaveSlot::Refresh()
{
    GameSaveData data;
    bool hasSave = SaveManager::LoadSave(slotId, data);

    if (!hasSave)
    {
        slotLabel->SetText("Slot " + std::to_string(slotId) + ": Empty");
    }
    else
    {
        slotLabel->SetText("Slot " + std::to_string(slotId) +
            "\nLevel: " + data.mapName +
            "\nSaved: " + data.timestamp +
            "\nPlayed: " + GameTime::FormatTime(data.timePlayedSeconds));
    }

    bool canSave = (mode == SaveSlotMode::Save) &&
        (GuardManager::GetInstance().GetAlertedGuardCount() == 0) &&
        (PlayerStrategyManager::GetCurrentStrategyType() == PlayerStrategyType::Normal);

    saveLoadBtn->SetActive(mode == SaveSlotMode::Load ? hasSave : canSave);
    deleteBtn->SetActive(hasSave);

    confirmPanel->SetVisible(false);
    saveLoadBtn->SetVisible(true);
    deleteBtn->SetVisible(hasSave);
}

void UISaveSlot::ApplyButtonConfigurations(const nlohmann::json& buttonJSON, std::function<void(const SystemAction&)> actionCallback)
{
    UIFactory::AttachButtonActions(saveLoadBtn, buttonJSON, actionCallback);
    UIFactory::AttachButtonActions(deleteBtn, buttonJSON, actionCallback);

    if (confirmPanel) {
        UIButton* yesBtn = confirmPanel->FindElementById<UIButton>("__" + id + "_yes_button");
        UIButton* noBtn = confirmPanel->FindElementById<UIButton>("__" + id + "_no_button");

        UIFactory::AttachButtonActions(yesBtn, buttonJSON, actionCallback);
        UIFactory::AttachButtonActions(noBtn, buttonJSON, actionCallback);
    }
}

uint64_t UISaveSlot::GetLastKnownRevision() const { return lastKnownRevision; }

void UISaveSlot::SetMode(SaveSlotMode newMode) { mode = newMode; }

SaveSlotMode UISaveSlot::GetMode() const { return mode; }

void UISaveSlot::SetSlotId(int newSlotId) { slotId = newSlotId; }

int UISaveSlot::GetSlotId() const { return slotId; }

void UISaveSlot::OnMainAction() const
{
    if (mode == SaveSlotMode::Load)
    {
        GameSaveData data;
        if (SaveManager::LoadSave(slotId, data))
        {
            if (!AppStateManager::GetInstance().IsCurrentStatePause()) 
            {
                AppStateManager::GetInstance().ScheduleStateChange(ENG_NEW(GameState));
                SaveManager::ScheduleApplySaveData(data);
            }
            else 
            {
                SaveManager::ForceApplySaveData(data);
                AppStateManager::GetInstance().SchedulePop();
            }
        }
    }
    else // Save Mode
    {
        SaveManager::SaveGame(SaveManager::CreateSave(slotId));
    }
}

void UISaveSlot::BuildConfirmPanel(TTF_Font* textFont, const SDL_Color& textColor, const UITextAlignment& textAlignment, const std::string& fontFamily)
{
    SDL_FRect confirmRect { bounds.x + bounds.w - 210, bounds.y + 55, 200, 50 };
    confirmPanel = ENG_NEW(UIPanel, 
        "__" + id + "_" + "confirm_panel",
        confirmRect, 
        backgroundColor, 
        borderColor
    );

    UILabel* label = ENG_NEW(UILabel,
        "__" + id + "_" + "confirm_label",
        SDL_FRect { confirmRect.x, confirmRect.y, confirmRect.w, 20 },
        backgroundColor,
        borderColor,
        "Are you sure?",
        textFont,
        textColor,
        textAlignment,
        fontFamily
    );

    UIButton* yesBtn = ENG_NEW(UIButton,
        "__" + id + "_" + "yes_button",
        SDL_FRect { confirmRect.x, confirmRect.y + 25, 95, 25 }, 
        backgroundColor,
        borderColor,
        "YES", 
        textFont,
        textColor,
        textAlignment,
        fontFamily
    );

    UIButton* noBtn = ENG_NEW(UIButton,
        "__" + id + "_" + "no_button",
        SDL_FRect { confirmRect.x + 100, confirmRect.y + 25, 95, 25 }, 
        backgroundColor,
        borderColor,
        "NO", 
        textFont,
        textColor,
        textAlignment,
        fontFamily
    );

    yesBtn->AddCallback([this]() {
        SaveManager::DeleteSave(slotId);
    }, ButtonCallbackType::Click);

    noBtn->AddCallback([this]() {
        confirmPanel->SetVisible(false);
        saveLoadBtn->SetVisible(true);
        deleteBtn->SetVisible(true);
    }, ButtonCallbackType::Click);

    label->SetBorderVisible(HIDE_BORDERS);
    yesBtn->SetBorderVisible(HIDE_BORDERS);
    noBtn->SetBorderVisible(HIDE_BORDERS);
    confirmPanel->SetBorderVisible(HIDE_BORDERS);

    confirmPanel->AddElement(label);
    confirmPanel->AddElement(yesBtn);
    confirmPanel->AddElement(noBtn);
    confirmPanel->SetVisible(false);

    AddElement(confirmPanel);
}