#pragma once

#ifndef UITABLE_H_
#define UITABLE_H_

#include <vector>

#include "UIElement.h"
#include "UITableColumn.h"
#include "UILabelPool.h"

class UITable : public UIElement
{
private:
    std::vector<UITableColumn> columns;    /// Vector of columns.
    std::vector<std::vector<std::string>> data; /// Vector of data for each column.

    UILabelPool headerPool; /// Label pool for the headers.
    UILabelPool cellPool;   /// Label pool for the cells row.

    UITextStyleElement headerTextStyle; /// Text style for the header.
    UITextStyleElement cellTextStyle;   /// Text style for the cells.

    SDL_Color headerBackgroundColor = { 0, 0, 0, 0 };   /// Color of the header background.
    SDL_Color cellBackgroundColor = { 0, 0, 0, 0 };   /// Color of the cell background.

    float scrollOffset = 0.0f;      /// Current scroll offset.
    float maxScrollOffset = 0.0f;   /// Maximum scroll offset.
    float rowHeight = 30.0f;        /// Row height of the table.
    float tablePadding = 5.0f;      /// Padding around the edges of the table for the text.
    bool needsUpdateFlag = false;   /// Flag if the table needs to be updated.
    
    /**
    * @brief Renders the extra table borders.
    * 
    * @param renderer Pointer to the SDL_Renderer used.
    */
    void RenderExtraBorders(SDL_Renderer* renderer) const;

    /**
    * @brief Rebuilds the table.
    * 
    * @throws 'std::runtime_error' if the number of rows and the number of columns is different.
    */
    void RebuildTable();

    /**
    * @brief Ensures the table is up to date, if not, rebuilds it.
    */
    void EnsureTable();

    /**
    * @brief Scrolls up the table.
    */
    void ScrollUp();

    /**
    * @brief Scrolls down the table.
    */
    void ScrollDown();

protected:

    /**
    * @brief Renders the table.
    *
    * @param renderer Pointer to the SDL Renderer used.
    */
    void OnRender(SDL_Renderer* renderer) const override;

public:

    /**
    * @brief Constructor.
    * 
    * @param id ID of the element.
    * @param bounds Bounds of the table.
    * @param backgroundColor Background color of the table.
    * @param borderColor Color of the borders.
    * @param headerBackgroundColor Background color of the header.
    * @param cellBackgroundColor Background color of the cells.
    * @param headerTextStyle Style of text for the header. Note that alignment is per whole column, so alignment here will have no effect.
    * @param cellTextStyle Style of text for the cells. Note that alignment is per whole column, so alignment here will have no effect.
    * @param rowHeight Height of a row.
    * @param tablePadding The padding of the text from the borders of the table.
    */
    explicit UITable(
        const std::string id = "",
        const SDL_FRect& bounds = { 0.0f, 0.0f, 0.0f, 0.0f },
        const SDL_Color& backgroundColor = { 0, 0, 0, 0 },
        const SDL_Color& borderColor = { 255, 255, 255, 255 },
        const SDL_Color& headerBackgroundColor = { 255, 255, 255, 255 },
        const SDL_Color& cellBackgroundColor = { 255, 255, 255, 255 },
        const UITextStyleElement& headerTextStyle = UITextStyleElement(),
        const UITextStyleElement& cellTextStyle = UITextStyleElement(),
        float rowHeight = 30.0f,
        float tablePadding = 5.0f
    );

    /**
    * @brief Handles an event.
    * 
    * @param e Event to be handled.
    */
    void HandleEvent(const SDL_Event& e) override;

    /**
    * @brief Updates the table.
    * 
    * @param deltaTime The delta time of the main game loop.
    */
    void Update(double deltaTime) override;

    /**
	* @brief Resizes the table and its contents.
    * 
	* @param widthScale The scale of the width (newWidth / oldWidth).
	* @param heightScale The scale of the height (newHeight / oldHeight).
    */
	void Resize(float widthScale, float heightScale) override;

    /**
    * @brief Invalides the table, requiring a rebuild.
    */
    void InvalidateTable();

    /**
    * @brief Sets the table columns.
    * 
    * @param newColumns New columns for the table.
    */
    void SetTableColumns(const std::vector<UITableColumn>& newColumns);

    /**
    * @brief Returns the table columns.
    * 
    * @return A reference to the vector of columns.
    */
    std::vector<UITableColumn>& GetTableColumns();

    /**
    * @brief Returns the table of columns.
    * 
    * @return A constant reference to the vector of columns.
    */
    const std::vector<UITableColumn>& GetTableColumns() const;

    /**
    * @brief Sets the table data.
    * 
    * @param newRows The new data for the rows of the table.
    */
    void SetTableData(const std::vector<std::vector<std::string>>& newRows);

    /**
    * @brief Returns the table data.
    * 
    * @return A reference to the table data.
    */
    std::vector<std::vector<std::string>>& GetTableData();

    /**
    * @brief Returns the table data.
    *
    * @return A constant reference to the table data.
    */
    const std::vector<std::vector<std::string>>& GetTableData() const;

    /**
    * @brief Sets the header text font.
    *
    * @param newFont New font of the header text.
    */
    void SetHeaderTextFont(TTF_Font* newFont);

    /**
    * @brief Returns the font of the header text.
    *
    * @return Pointer to the TTF_Font of the header text.
    */
    TTF_Font* GetHeaderTextFont() const;

    /**
    * @brief Sets the cell text font.
    * 
    * @param newFont New font of the cell text.
    */
    void SetCellTextFont(TTF_Font* newFont);

    /**
    * @brief Returns the font of the cell text.
    * 
    * @return Pointer to the TTF_Font of the cell text.
    */
    TTF_Font* GetCellTextFont() const;

    /**
    * @brief Sets the header text color.
    * 
    * @param newColor New color of the header text.
    */
    void SetHeaderTextColor(const SDL_Color& newColor);

    /**
    * @brief Returns the color of the header text.
    * 
    * @return The SDL_Color structure of the header text.
    */
    SDL_Color GetHeaderTextColor() const;

    /**
    * @brief Sets the header background color.
    *
    * @param newColor New color of the header background.
    */
    void SetHeaderBackgroundColor(const SDL_Color& newColor);

    /**
    * @brief Returns the color of the header background.
    *
    * @return The SDL_Color structure of the header background.
    */
    SDL_Color GetHeaderBackgroundColor() const;

    /**
    * @brief Sets the cell text color.
    *
    * @param newColor New color of the cell text.
    */
    void SetCellTextColor(const SDL_Color& newColor);

    /**
    * @brief Returns the color of the cell text.
    *
    * @return The SDL_Color structure of the cell text.
    */
    SDL_Color GetCellTextColor() const;

    /**
    * @brief Sets the cell background color.
    *
    * @param newColor New color of the cell background.
    */
    void SetCellBackgroundColor(const SDL_Color& newColor);

    /**
    * @brief Returns the color of the cell background.
    *
    * @return The SDL_Color structure of the cell background.
    */
    SDL_Color GetCellBackgroundColor() const;

    /**
    * @brief Sets the color of the borders.
    *
    * @param newColor New color of the borders.
    */
    void SetBorderColor(const SDL_Color& newColor);

    /**
    * @brief Returns the border color of the table.
    *
    * @return The SDL_Color of the border.
    */
    SDL_Color GetBorderColor() const;

    /**
    * @brief Sets the bounds of the table.
    * 
    * @param newBounds New bounds of the table.
    */
    void SetBounds(const SDL_FRect& newBounds) override;
};

#endif // UITABLE_H_
