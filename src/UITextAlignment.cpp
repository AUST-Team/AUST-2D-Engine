#include <SDL3/SDL_log.h>

#include "UITextAlignment.h"
#include "StringMiscs.h"

SDL_FPoint GetAlignedPosition(const SDL_FRect& bounds, float textWidth, float textHeight, const UITextAlignment& alignment, float totalBlockHeight, float padding)
{
    if (totalBlockHeight <= 0.0f)
    {
        totalBlockHeight = textHeight;
    }

    float x = 0.0f;
    float y = 0.0f;

    switch (alignment.horizontalAlignment)
    {
        case HorizontalAlignment::Left:
        {
            x = bounds.x + padding;
            break;
        }

        case HorizontalAlignment::Center:
        {
            x = bounds.x + (bounds.w - textWidth) / 2.0f;
            break;
        }

        case HorizontalAlignment::Right:
        {
            x = bounds.x + bounds.w - textWidth - padding;
            break;
        }

        default:
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GetAlignedPosition: Unknown horizontal alignment.");
            break;
		}
    }

    switch (alignment.verticalAlignment)
    {
        case VerticalAlignment::Top:
        {
            y = bounds.y + padding;
            break;
        }

        case VerticalAlignment::Middle:
        {
            y = bounds.y + (bounds.h - totalBlockHeight) / 2.0f;
            break;
        }

        case VerticalAlignment::Bottom:
        {
            y = bounds.y + bounds.h - totalBlockHeight - padding;
            break;
        }

        default:
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GetAlignedPosition: Unknown vertical alignment.");
            break;
		}
    }

    return SDL_FPoint { x, y };
}

VerticalAlignment VerticalAlignmentFromString(const std::string& vertAlignment)
{
    const std::string& alignLwr = StringMiscs::ToLower(vertAlignment);

#define X(name, val, str)   \
    if (alignLwr == str)    \
        return VerticalAlignment::name;

    VERTICAL_ALIGNMENT_LIST(X)

#undef X

    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "VerticalAlignmentFromString: Unknown type [%s]. Defaulting to \"%s\"", vertAlignment.c_str(), ConstantConfiguration::enumSentinelString);
    return GetSentinel<VerticalAlignment>();
}

std::string VerticalAlignmentToString(VerticalAlignment vertAlignment)
{
    switch (vertAlignment)
    {
#define X(name, val, str)   \
        case VerticalAlignment::name: return str;

        VERTICAL_ALIGNMENT_LIST(X)

#undef X

        default:
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "VerticalAlignmentToString: Unknown vertical alignment. Defaulting to \"%s\"", ConstantConfiguration::enumSentinelStringLower);
			return ConstantConfiguration::enumSentinelStringLower;
        }
    }
}

HorizontalAlignment HorizontalAlignmentFromString(const std::string& horizAlignment)
{
    const std::string& alignLwr = StringMiscs::ToLower(horizAlignment);

#define X(name, val, str)   \
    if (alignLwr == str)    \
        return HorizontalAlignment::name;

    HORIZONTAL_ALIGNMENT_LIST(X)

#undef X

    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "HorizontalAlignmentFromString: Unknown type [%s]. Defaulting to \"%s\"", horizAlignment.c_str(), ConstantConfiguration::enumSentinelString);
    return GetSentinel<HorizontalAlignment>();
}

std::string HorizontalAlignmentToString(HorizontalAlignment horizAlignment)
{
    switch (horizAlignment)
    {
#define X(name, val, str)   \
        case HorizontalAlignment::name: return str;

        HORIZONTAL_ALIGNMENT_LIST(X)

#undef X

        default:
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "HorizontalAlignmentToString: Unknown horizontal alignment. Defaulting to \"%s\"", ConstantConfiguration::enumSentinelStringLower);
            return ConstantConfiguration::enumSentinelStringLower;
        }
    }
}

bool UITextAlignment::operator==(const UITextAlignment& other) const
{
    return horizontalAlignment == other.horizontalAlignment && verticalAlignment == other.verticalAlignment;
}

bool UITextAlignment::operator!=(const UITextAlignment& other) const { return !(*this == other); }
