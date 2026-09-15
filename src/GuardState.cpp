#include "GuardState.h"

#include "Unused.h"

void GuardState::Exit(Guard& guard) { UNUSED(guard); }

bool GuardState::IsShared() const { return true; }