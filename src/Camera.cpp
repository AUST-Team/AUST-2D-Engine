#include <SDL3/SDL.h>
#include <algorithm>
#include <stdexcept>
#include <cmath>

#include "Camera.h"

#include "GameMap.h"
#include "Entity.h"
#include "MemoryTracker.h"
#include "Unused.h"

Camera* Camera::cameraInstance = nullptr;
bool Camera::created = false;

Camera::Camera(int width, int height) :
    windowWidth(width), 
    windowHeight(height) 
{ 
    const GameConfiguration& config = Configuration::Get();
    const TileConfiguration& tileConfig = config.tiles;

    safeScales = CalculateSafeScales(tileConfig.width, tileConfig.height, tileConfig.minScale, tileConfig.maxScale, tileConfig.step, ConstantConfiguration::epsilon);

    RecalculateScale(); 
}

Camera* Camera::CreateInstance(int width, int height) 
{
    if (!created && !cameraInstance) 
    {
        cameraInstance = ENG_NEW(Camera, width, height);
        created = true;
    }

    return cameraInstance;
}

Camera* Camera::GetInstance() { return cameraInstance; }

void Camera::DeleteInstance() 
{
    if (created && cameraInstance) 
    {
        ENG_DELETE(cameraInstance);
        cameraInstance = nullptr;
        created = false;
    }
    else
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Camera.DeleteInstance: Camera was already deleted or doesn't exist.");
    }
}

void Camera::Update(double deltaTime, const Entity& focus)
{
    UNUSED(deltaTime);
    const int viewportWidth = GetViewportWidth();
    const int viewportHeight = GetViewportHeight();

	const GameMap& gameMap = GameMap::GetInstance();
    const int mapWidth = gameMap.GetMapWidthPixels();
    const int mapHeight = gameMap.GetMapHeightPixels();

    // If both the map width and height are smaller than the viewport, don't clamp or follow the player.
    if (mapWidth < viewportWidth && mapHeight < viewportHeight)
    {
        CenterOnMap(mapWidth, mapHeight);
        return;
    }

    // Get entity center.
    const float playerCenterX = focus.GetX() + focus.GetWidth() / 2.0f;
    const float playerCenterY = focus.GetY() + focus.GetHeight() / 2.0f;

    const float targetX = playerCenterX - viewportWidth / 2.0f;
    const float targetY = playerCenterY - viewportHeight / 2.0f;

    Clamp(mapWidth, mapHeight);

    if (firstUpdate)
    {
        xOffset = targetX;
        yOffset = targetY;
        firstUpdate = false;
    }
    else
    {
        xOffset = std::lerp(xOffset, targetX, smoothing);
        yOffset = std::lerp(yOffset, targetY, smoothing);
    }

}

void Camera::Clamp(int mapWidth, int mapHeight)
{
    const int viewportWidth = GetViewportWidth();
    const int viewportHeight = GetViewportHeight();

    const float maxX = static_cast<float> (mapWidth) - viewportWidth;
    const float maxY = static_cast<float> (mapHeight) - viewportHeight;

    // If the map width is bigger than the viewport, then clamp.
    if (mapWidth > viewportWidth)
    {
        if (xOffset < 0)
        {
            xOffset = 0;
        }
        else if (xOffset > maxX)
        {
            xOffset = maxX;
        }
    }

    // If the map height is bigger than the viewport, then clamp.
    if (mapHeight > viewportHeight)
    {
        if (yOffset < 0)
        {
            yOffset = 0;
        }
        else if (yOffset > maxY)
        {
            yOffset = maxY;
        }
    }
}

void Camera::CenterOnMap(int mapWidth, int mapHeight)
{
    const float viewportWidth = static_cast<float>(GetViewportWidth());
    const float viewportHeight = static_cast<float>(GetViewportHeight());

    xOffset = (mapWidth < viewportWidth)
        ? -(viewportWidth - mapWidth) / 2.0f
        : 0.0f;

    yOffset = (mapHeight < viewportHeight)
        ? -(viewportHeight - mapHeight) / 2.0f
        : 0.0f;
}

void Camera::SetWindowSize(int newWidth, int newHeight)
{
    if (windowWidth == newWidth && windowHeight == newHeight)
    {
        return;
    }

    windowWidth = newWidth;
    windowHeight = newHeight;
    RecalculateScale();
}

int Camera::WorldToScreenX(float worldX) const { return static_cast<int>(std::round( (worldX - xOffset) * scale) ); }

int Camera::WorldToScreenY(float worldY) const { return static_cast<int>(std::round( (worldY - yOffset) * scale) ); }

int Camera::GetViewportWidth() const { return static_cast<int>(windowWidth / scale); }

int Camera::GetViewportHeight() const { return static_cast<int>(windowHeight / scale); }

float Camera::GetXOffset() const { return xOffset; }

float Camera::GetYOffset() const { return yOffset; }

float Camera::GetScale() const { return scale; }

void Camera::SetFirstUpdate(bool value) { firstUpdate = value; }

void Camera::RecalculateScale()
{
    const GameConfiguration& config = Configuration::Get();
    const int baseWidth = config.window.width;
    const int baseHeight = config.window.height;

    const float scaleX = static_cast<float>(windowWidth) / baseWidth;
    const float scaleY = static_cast<float>(windowHeight) / baseHeight;
    const float maxAllowedScale = std::min(scaleX, scaleY);

    scale = safeScales.front();
    for (const float s : safeScales)
    {
        if (s <= maxAllowedScale)
        {
            scale = s;
        }
        else
        {
            break;
        }
    }
}

std::vector<float> Camera::CalculateSafeScales(int tileWidth, int tileHeight, float minScale, float maxScale, float step, float eps)
{
    std::vector<float> validScales;

    if (tileWidth <= 0 || tileHeight <= 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Camera.CalculateSafeScales: Invalid tile dimensions (%d x %d)", tileWidth, tileHeight);
        return { 1.0f };
    }

    for (float testedScale = minScale; testedScale <= maxScale + eps; testedScale += step)
    {
        const float scaledW = tileWidth * testedScale;
        const float scaledH = tileHeight * testedScale;

        const bool isWidthInteger = std::abs(scaledW - std::round(scaledW)) < eps;
        const bool isHeightInteger = std::abs(scaledH - std::round(scaledH)) < eps;

        if (isWidthInteger && isHeightInteger)
        {
            validScales.push_back(testedScale);
        }
    }

    if (validScales.empty())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Camera.CalculateSafeScales: No scale valid was found. Returning 1.0f");
        validScales.push_back(1.0f);
    }

    return validScales;
}
