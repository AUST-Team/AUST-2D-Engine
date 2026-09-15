#include <stdexcept>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "UITable.h"

#include "SDL_ColorOperators.h"
#include "SDL_FRectOperators.h"
#include "Configuration.h"
#include "Miscs.h"
#include "FontManager.h"
#include "StringMiscs.h"
#include "Unused.h"

UITable::UITable(const std::string id, const SDL_FRect& bounds, const SDL_Color& backgroundColor, const SDL_Color& borderColor, const SDL_Color& headerBackgroundColor, const SDL_Color& cellBackgroundColor, const UITextStyleElement& headerTextStyle, const UITextStyleElement& cellTextStyle, float rowHeight, float tablePadding) :
    UIElement(id, bounds, backgroundColor, borderColor),
    headerTextStyle(headerTextStyle),
    cellTextStyle(cellTextStyle),
    rowHeight(rowHeight),
    headerBackgroundColor(headerBackgroundColor),
    cellBackgroundColor(cellBackgroundColor),
    tablePadding(tablePadding) {}

void UITable::HandleEvent(const SDL_Event& e)
{
    if (e.type == SDL_EVENT_MOUSE_WHEEL) 
    {
        float mx, my;
        SDL_GetMouseState(&mx, &my);

        if (Contains(mx, my)) 
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
}

void UITable::Update(double deltaTime)
{
    UNUSED(deltaTime);
    if (needsUpdateFlag)
    {
        EnsureTable();
    }
}

void UITable::Resize(float widthScale, float heightScale)
{
    UIElement::Resize(widthScale, heightScale);

    float fontScale = std::min(widthScale, heightScale);
    float newCellFontSize = cellTextStyle.GetOriginalFontSize() * fontScale;
    float newHeaderFontSize = headerTextStyle.GetOriginalFontSize() * fontScale;

    TTF_Font* newCellFont = FontManager::GetInstance().GetFontByFamily(cellTextStyle.GetFontFamily(), newCellFontSize);
	TTF_Font* newHeaderFont = FontManager::GetInstance().GetFontByFamily(headerTextStyle.GetFontFamily(), newHeaderFontSize);

    if (!newCellFont || !newHeaderFont)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITable.Resize: Failed to load new fonts: %s", SDL_GetError());
        return;
    }

    SetCellTextFont(newCellFont);
	SetHeaderTextFont(newHeaderFont);
	RebuildTable();
}

void UITable::InvalidateTable() { needsUpdateFlag = true; }

void UITable::SetTableColumns(const std::vector<UITableColumn>& newColumns)
{
	if (columns != newColumns)
	{
		columns = newColumns;
		RebuildTable();
	}
}

std::vector<UITableColumn>& UITable::GetTableColumns() { return columns; }

const std::vector<UITableColumn>& UITable::GetTableColumns() const { return columns; }

void UITable::SetTableData(const std::vector<std::vector<std::string>>& newRows)
{
	if (data != newRows)
	{
		data = newRows;
		RebuildTable();
	}
}

std::vector<std::vector<std::string>>& UITable::GetTableData() { return data; }

const std::vector<std::vector<std::string>>& UITable::GetTableData() const { return data; }

void UITable::SetHeaderTextFont(TTF_Font* newFont)
{
    if (headerTextStyle.GetTextFont() != newFont)
    {
        headerTextStyle.SetTextFont(newFont);
        RebuildTable();
    }
}

TTF_Font* UITable::GetHeaderTextFont() const { return headerTextStyle.GetTextFont(); }

void UITable::SetCellTextFont(TTF_Font* newFont)
{
	if (cellTextStyle.GetTextFont() != newFont)
	{
        cellTextStyle.SetTextFont(newFont);
		RebuildTable();
	}
}

TTF_Font* UITable::GetCellTextFont() const { return cellTextStyle.GetTextFont(); }

void UITable::SetHeaderTextColor(const SDL_Color& newColor)
{
    if (headerTextStyle.GetTextColor() != newColor)
    {
        headerTextStyle.SetTextColor(newColor);
        RebuildTable();
    }
}

SDL_Color UITable::GetHeaderTextColor() const { return headerTextStyle.GetTextColor(); }

void UITable::SetHeaderBackgroundColor(const SDL_Color& newColor) { headerBackgroundColor = newColor; }

SDL_Color UITable::GetHeaderBackgroundColor() const { return headerBackgroundColor; }

void UITable::SetCellTextColor(const SDL_Color& newColor)
{
    if (cellTextStyle.GetTextColor() != newColor)
    {
        cellTextStyle.SetTextColor(newColor);
        RebuildTable();
    }
}

SDL_Color UITable::GetCellTextColor() const { return cellTextStyle.GetTextColor(); }

void UITable::SetCellBackgroundColor(const SDL_Color& newColor)
{
    if (cellBackgroundColor != newColor)
    {
        cellBackgroundColor = newColor;
        RebuildTable();
    }
}

SDL_Color UITable::GetCellBackgroundColor() const { return cellBackgroundColor; }

void UITable::SetBorderColor(const SDL_Color& newColor) { borderColor = newColor; }

SDL_Color UITable::GetBorderColor() const { return borderColor; }

void UITable::SetBounds(const SDL_FRect& newBounds)
{
	if (bounds != newBounds)
	{
        UIElement::SetBounds(newBounds);
        RebuildTable();
	}
}

void UITable::RenderExtraBorders(SDL_Renderer* renderer) const
{
    SDL_FRect tableBounds = GetBounds();

    float totalWidth = tableBounds.w;
    float currentX = tableBounds.x;

    if (!SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITable.RenderExtraBorders: Failed SetRenderDrawColor: %s", SDL_GetError());
    }

    for (size_t i = 0; i < columns.size(); ++i)
    {
        currentX += columns[i].widthRatio * totalWidth;

        if (i < columns.size() - 1)
        {
            // - 1 comes from... No idea, maybe SDL anti-aliasing, it would always stick out one pixel past the bottom of the table. No idea why.
            if (!SDL_RenderLine(renderer, currentX, tableBounds.y, currentX, tableBounds.y + tableBounds.h - 1))
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITable.RenderExtraBorders: Failed RenderLine for [%llu] line: %s", i, SDL_GetError());
                continue;
            }
        }
    }

    const float lineY = bounds.y + rowHeight + 2 * tablePadding;

    SDL_RenderLine(
        renderer,
        bounds.x,
        lineY,
        bounds.x + bounds.w,
        lineY
    );
}

void UITable::RebuildTable()
{
    const size_t columnCount = columns.size();
    const size_t rowCount = data.size();

    for (const std::vector<std::string>& row : data) 
    {
        if (row.size() != columnCount) 
        {
			SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "UITable.RebuildTable: Row size (%llu) does not match column count (%llu). Aborting table build.", row.size(), columnCount);
            return;
        }
    }

    const size_t fontHeight = cellTextStyle.GetCurrentFontHeight();
    const float tableRowHeight = std::max(rowHeight, static_cast<float>(fontHeight) * 1.2f);

    headerPool.Resize(columnCount);
    cellPool.Resize(rowCount * columnCount);
    headerPool.SetBorderColor({ 0, 0, 0, 0 });
    cellPool.SetBorderColor({ 0, 0, 0, 0 });

    const float tableWidth = bounds.w;

    float x = bounds.x;
    float y = bounds.y;

    headerPool.SetTextFont(headerTextStyle.GetTextFont());
    headerPool.SetTextColor(headerTextStyle.GetTextColor());

    for (size_t i = 0; i < columnCount; ++i) 
    {
        UILabel& header = headerPool[i];

        const float colWidth = columns[i].widthRatio * tableWidth;
        const float paddedWidth = colWidth - tablePadding;

        header.SetBounds({ x + tablePadding, y, paddedWidth, tableRowHeight });

        std::string text = StringMiscs::TruncateTextToFit(columns[i].header, headerTextStyle.GetTextFont(), static_cast<int>(paddedWidth));
        header.SetText(text);

        header.SetTextAlignment(columns[i].alignment);
        x += colWidth;
    }

    y += tableRowHeight + 2 * tablePadding;

    for (size_t row = 0; row < rowCount; ++row) 
    {
        x = bounds.x;
        cellPool.SetTextColor(cellTextStyle.GetTextColor());
        cellPool.SetTextFont(cellTextStyle.GetTextFont());
        for (size_t col = 0; col < columnCount; ++col) 
        {
            size_t index = row * columnCount + col;
            UILabel& cell = cellPool[index];

            float colWidth = columns[col].widthRatio * tableWidth;
            float paddedWidth = colWidth - 2 * tablePadding;

            cell.SetBounds({ x + tablePadding, y, paddedWidth, tableRowHeight });

            const std::string& rawText = data[row][col];
            std::string truncated = StringMiscs::TruncateTextToFit(rawText, cellTextStyle.GetTextFont(), static_cast<int>(paddedWidth));
            cell.SetText(truncated);

            cell.SetTextAlignment(columns[col].alignment);

            x += colWidth;
        }
        y += tableRowHeight;
    }

    const float visibleHeight = bounds.h - tableRowHeight;
    const float totalRowHeight = static_cast<float>(data.size()) * tableRowHeight;

    maxScrollOffset = std::max(0.0f, totalRowHeight - visibleHeight + 2 * tablePadding);
    scrollOffset = std::min(scrollOffset, maxScrollOffset);
}

void UITable::EnsureTable()
{
	if (needsUpdateFlag)
	{
		RebuildTable();
		needsUpdateFlag = false;
	}
}

void UITable::ScrollUp() 
{
    if (scrollOffset > 0.0f) 
    {
        scrollOffset -= rowHeight;
        if (scrollOffset < 0.0f)
        {
            scrollOffset = 0.0f;
        }
    }
}

void UITable::ScrollDown() 
{
    if (scrollOffset < maxScrollOffset) 
    {
        scrollOffset += rowHeight;
        if (scrollOffset > maxScrollOffset)
        {
            scrollOffset = maxScrollOffset;
        }
    }
}

void UITable::OnRender(SDL_Renderer* renderer) const
{
    SDL_Rect clipRect {
        static_cast<int>(bounds.x),
        static_cast<int>(bounds.y + rowHeight + 2 * tablePadding),
        static_cast<int>(bounds.w),
        static_cast<int>(bounds.h - rowHeight - 2 * tablePadding)
    };

    if (headerBackgroundColor.a > 0)
    {
        SDL_FRect headerRect {
            bounds.x,
            bounds.y,
            bounds.w,
            rowHeight + 2 * tablePadding
        };

        if (!SDL_SetRenderDrawColor(renderer, headerBackgroundColor.r, headerBackgroundColor.g, headerBackgroundColor.b, headerBackgroundColor.a))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITable.OnRender: Failed SetRenderDrawColor: %s", SDL_GetError());
        }

        if (!SDL_RenderFillRect(renderer, &headerRect))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITable.OnRender: Failed RenderRect: %s", SDL_GetError());
        }
    }

    headerPool.Render(renderer);

    if (showBordersFlag)
    {
        RenderExtraBorders(renderer);
    }

    if (!SDL_SetRenderClipRect(renderer, &clipRect))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UITable.OnRender: Failed SetRenderClipRect: %s", SDL_GetError());
        return;
    }

    cellPool.Render(renderer, -scrollOffset);

    if (!SDL_SetRenderClipRect(renderer, nullptr))
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "UITable.OnRender: Failed SetRenderClipRect: %s", SDL_GetError());
        return;
    }
}

