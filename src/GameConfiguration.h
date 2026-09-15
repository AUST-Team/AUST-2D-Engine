#pragma once

#ifndef GAMECONFIGURATION_H_
#define GAMECONFIGURATION_H_

#include "WindowConfiguration.h"
#include "AppConfiguration.h"
#include "CharacterConfiguration.h"
#include "TileConfiguration.h"
#include "PlayerConfiguration.h"
#include "GuardSearchConfiguration.h"
#include "GuardConfiguration.h"
#include "UIColorConfiguration.h"
#include "ConditionConfiguraiton.h"
#include "SpatialBucketConfiguration.h"
#include "IOConfiguration.h"

/**
* @brief Structure holding all other configurations.
*/
struct GameConfiguration
{
    WindowConfiguration window; /// Window configuration.
    AppConfiguration app;       /// App configuration.

    TileConfiguration tiles;            /// Tile configuration.
    CharacterConfiguration character;   /// Character configuration.
    PlayerConfiguration player;         /// Player configuration.

    GuardConfiguration guard;               /// Guard configuration.
    GuardSearchConfiguration guardSearch;   /// Guard search configuration.

    UIColorConfiguration ui;             /// UI configuration.

    ConditionConfiguration conditions;          /// Condition configuration.
    SpatialBucketConfiguration spatialBuckets;  /// Spatial bucket configuration.
        
    IOConfiguration paths;  /// IO (paths) configuration
};

#endif // GAMECONFIGURATION_H_