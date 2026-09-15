#pragma once

#ifndef WRITERSTATETYPE_H_
#define WRITERSTATETYPE_H_

#include <cstdint>

/**
* @brief State types for a cutscene text writer.
*/
enum class WriterStateType : uint8_t
{
    Typing,     /// Typing out current line character by character.
    Waiting,    /// Waiting for player input or timer to advance.
    FadingOut,  /// Transitioning/fading out before completing.
    Finished,   /// Done completely.
    COUNT       /// Number of elements.
};


#endif // WRITERSTATETYPE_H_