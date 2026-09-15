#include "MapTransitionInteractable.h"

#include "GameMap.h"

void MapTransitionInteractable::Interact() { GameMap::GetInstance().LoadMapFromName(mapName); }
