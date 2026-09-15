#pragma once

#ifndef UISAVESLOT_H_
#define UISAVESLOT_H_

#include <string>
#include <nlohmann/json.hpp>

#include "UIPanel.h"
#include "SaveSlotMode.h"
#include "UITextAlignment.h"

class UILabel;
class UIButton;
struct TTF_Font;
struct SDL_Color;
struct SystemAction;

/**
* @brief A save slot.
*/
class UISaveSlot : public UIPanel
{
private:
    UILabel* slotLabel = nullptr;       /// Slot label. Owning.
    UIButton* saveLoadBtn = nullptr;    /// Save / Load button. Owning.
    UIButton* deleteBtn = nullptr;      /// Delete button. Owning.
    UIPanel* confirmPanel = nullptr;    /// Confirm panel. Owning.

    uint64_t lastKnownRevision = 0;     /// Last known slot revision.
    SaveSlotMode mode = SaveSlotMode::Save;     /// Mode of the save slot.
    int slotId = 1;    /// The ID of the save slot.

    /**
    * @brief Performs the main action of the save / load slot (loading or saving).
    */
    void OnMainAction() const;

    /**
    * @brief Builds the confirmation panel.
    *
    * @param textFont The font of the text.
    * @param textColor The color of the text.
    * @param textAlignment Alignment of text.
    * @param fontFamily Font family name of the text font.
    */
    void BuildConfirmPanel(
        TTF_Font* textFont = nullptr,
        const SDL_Color& textColor = { 255, 255, 255, 255 },
        const UITextAlignment& textAlignment = { HorizontalAlignment::Center, VerticalAlignment::Middle },
        const std::string& fontFamily = ""
    );

public:

    /**
    * @brief Constructor.
    *
    * @param id ID of the element.
    * @param bounds The bounds of the panel.
    * @param backgroundColor Background color of the panel.
    * @param borderColor Border color of the panel.
    * @param textFont The font of the text.
    * @param textColor The color of the text.
    * @param textAlignment Alignment of text.
	* @param fontFamily Font family name of the text font.
    * @param slotId The ID of the slot.
    * @param mode Mode of the slot (save or load).
    */
    UISaveSlot(
        const std::string id = "",
        const SDL_FRect& bounds = { 0.0f, 0.0f, 0.0f, 0.0f },
        const SDL_Color& backgroundColor = { 0, 0, 0, 0 },
        const SDL_Color& borderColor = { 255, 255, 255, 255 },
        TTF_Font* textFont = nullptr,
        const SDL_Color& textColor = { 255, 255, 255, 255 },
        const UITextAlignment& textAlignment = { HorizontalAlignment::Center, VerticalAlignment::Middle },
        const std::string& fontFamily = "",
        int slotId = 1,
        SaveSlotMode mode = SaveSlotMode::Save
    );

    /**
    * @brief Updates the UI.
    * 
    * @param deltaTime The deltaTime of the main game loop.
    */
    void Update(double deltaTime) override;

    /**
    * @brief Refreshes the save slot.
    */
    void Refresh();

    /**
	* @brief Applies button configurations from a JSON object to the internal buttons.
    * 
	* @param buttonJSON The JSON object containing button configurations.
	* @param actionCallback A callback function to be called when a button is clicked.
    */
    void ApplyButtonConfigurations(const nlohmann::json& buttonJSON, std::function<void(const SystemAction&)> actionCallback);

    /**
    * @brief Returns the last known slot revision.
    *
    * @return The last known revision.
    */
    uint64_t GetLastKnownRevision() const;

    /**
    * @brief Sets the mode of the save slot.
    *
    * @param newMode The new mode for the save slot.
    */
    void SetMode(SaveSlotMode newMode);

    /**
    * @brief Returns the mode of the save slot.
    *
    * @return The current SaveSlotMode.
    */
    SaveSlotMode GetMode() const;

    /**
    * @brief Sets the slot ID.
    *
    * @param newSlotId The new ID for the slot.
    */
    void SetSlotId(int newSlotId);

    /**
    * @brief Returns the slot ID.
    *
    * @return The ID of the slot.
    */
    int GetSlotId() const;
};

#endif // UISAVESLOT_H_