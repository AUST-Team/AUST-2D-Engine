#include <SDL3/SDL_log.h>

#include "RenderLayer.h"

#include "StringMiscs.h"

RenderLayer RenderLayerFromString(const std::string& layer)
{
	const std::string& layerLwr = StringMiscs::ToLower(layer);

#define X(name, val, str)   \
    if (layerLwr == str)    \
        return RenderLayer::name;

	RENDER_LAYER_LIST(X)

#undef X

	SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "RenderLayerFromString: Invalid layer name [%s]. Defaulting to \"%s\"", layer.c_str(), ConstantConfiguration::enumSentinelString);
	return GetSentinel<RenderLayer>();;
}

std::string RenderLayerToString(RenderLayer layer)
{
    switch (layer)
    {
#define X(name, val, str) \
        case RenderLayer::name: return str;

        RENDER_LAYER_LIST(X)

#undef X

        default:
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "RenderLayerToString: Unknown render layer. Defaulting to \"%s\"", ConstantConfiguration::enumSentinelStringLower);
            return ConstantConfiguration::enumSentinelStringLower;
        }
    }
}
