#include "GuardConfiguration.h"

VisionRange GuardConfiguration::MakeVisionRange(const VisionRangeModifierConfiguration& mod) const
{
    return VisionRange(
        vision.base.centralAngle + mod.additionalAngle,
        vision.base.coneAngle + mod.additionalAngle,
        vision.base.peripheralAngle + mod.additionalAngle,
        vision.base.centralRange + mod.additionalTiles,
        vision.base.coneRange + mod.additionalTiles,
        vision.base.peripheralRange + mod.additionalTiles
    );
}
