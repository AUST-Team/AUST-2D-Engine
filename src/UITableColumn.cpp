#include "UITableColumn.h"

bool UITableColumn::operator==(const UITableColumn& other) const
{
    return header == other.header &&
        widthRatio == other.widthRatio &&
        alignment == other.alignment;
}

bool UITableColumn::operator!=(const UITableColumn& other) const
{
    return !(*this == other);
}
