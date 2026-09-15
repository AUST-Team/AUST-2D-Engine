#pragma once

#ifndef UITABLECOLUMN_H_
#define UITABLECOLUMN_H_

#include <string>

#include "UITextAlignment.h"

/**
* @brief Structure for a column in the UITable.
*/
struct UITableColumn
{
    std::string header;         /// Text of the column header.
    float widthRatio;           /// Width of column as a ratio.
    UITextAlignment alignment;  /// Alignment of the column.

    /**
    * @brief Equality operator.
    */
    bool operator==(const UITableColumn& other) const;

    /**
    * @brief Non-equality operator.
    */
    bool operator!=(const UITableColumn& other) const;
};

#endif // UITABLECOLUMN_H_
