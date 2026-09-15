#pragma once

#ifndef CAMERA_H_
#define CAMERA_H_

#include <vector>

#include "Observer.h"
#include "Configuration.h"

/**
* @brief Camera class. Encapsulates the camera itself, also the scale of drawing.
* 
* Uses world units (non-distorted sizes) and game units (distorted sizes).
*/
class Camera : public Observer 
{
private:
    static Camera* cameraInstance;  /// Pointer to a camera to keep a single instance at any time.
    static bool created;            /// Boolean flag if the instance has been created or not.

    float xOffset = 0.0f;           /// The X offset of the camera (X coordinate of the upper left corner).
    float yOffset = 0.0f;           /// The Y offset of the camera (Y coordinate of the upper left corner).
    float scale = 1.0f;             /// The scale of the camera (how big or small stuff gets drawn on screen).
    float smoothing = 0.1f;         /// The smoothing factor of the camera. Used for LERP.

    int windowWidth = Configuration::Get().window.width;     /// Camera's window width.
    int windowHeight = Configuration::Get().window.height;   /// Camera's window height.

    bool firstUpdate = true;        /// Boolean flag if this is the first update of the camera. Used for LERP.

    std::vector<float> safeScales = { 0.5f, 0.75f, 1.f, 1.25f, 1.5f, 2.f, 2.5f, 3.f, 4.f }; /// Vector of safe scales for 48x48 tiles.

    // No copying or moving allowed due to singleton pattern.
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera(Camera&&) = delete;
    Camera& operator=(Camera&&) = delete;

    /**
    * @brief Recalculates the most fitting scale for the current window size.
    */
    void RecalculateScale();

    /**
     * @brief Dynamically calculates scales that result in exact integer pixel
     * dimensions for the configured tile size to prevent sub-pixel bleeding.
     *
     * @param tileWidth Width of a tile in world units (usually defined in the configuration file).
     * @param tileHeight Height of a tile in world units (usually defined in the configuration file).
     * @param minScale Minimum allowable scale factor (e.g., 0.25f).
     * @param maxScale Maximum allowable scale factor (e.g., 4.0f).
     * @param step The granularity increment to test against (e.g., 0.05f or 0.125f).
     * @param eps Epsilon value.
     * 
     * @return std::vector<float> List of valid integer-aligned scale factors.
     */
    std::vector<float> CalculateSafeScales(int tileWidth, int tileHeight, float minScale, float maxScale, float step, float eps = ConstantConfiguration::epsilon);

public:

    /**
    * @brief Constructor. Private for singleton pattern.
    *
    * @param width Integer for the width of the camera.
    * @param height Integer for the height of the camera.
    */
    explicit Camera(int width = Configuration::Get().window.width, int height = Configuration::Get().window.height);

    /**
    * @brief Default destructor.
    *
    * Deletion is done through the DeleteInstance method.
    */
    ~Camera() override = default;

    /**
    * @brief Static method to create a camera instance.
    * 
    * @param width Integer for the width of the camera.
    * @param height Integer for the height of the camera.
    * 
    * @return Pointer to the created instance.
    */
    static Camera* CreateInstance(int width = Configuration::Get().window.width, int height = Configuration::Get().window.height);

    /**
    * @brief Static method to get the camera instance;
    * 
    * @returns A pointer to the camera instance.
    */
    static Camera* GetInstance();

    /**
    * @brief Static method to delete the camera instance.
    */
    static void DeleteInstance();

    /**
    * @brief Updates the camera
    * 
    * @param deltaTime The delta time from the main SDL loop.
    * @param subject The subject whose attributes will be used.
    */
    void Update(double deltaTime, const Entity& focus) override;

    /**
    * @brief Clamps the camera to the map width and height
    * 
    * @param mapWidth The width of the map in pixels
    * @param mapHeight The height of the map in pixels
    */
    void Clamp(int mapWidth, int mapHeight);

    /**
    * @brief Centers the camera on the map center.
    * 
    * @param mapWidth The width of the map in pixels
    * @param mapHeight The height of the map in pixels
    */
    void CenterOnMap(int mapWidth, int mapHeight);

    /**
    * @brief Sets the camera to a new window size. Recalculates scale.
    * 
    * @param newWidth New width of the camera.
    * @param newHeight New height of the camera.
    */
    void SetWindowSize(int newWidth, int newHeight);

    /**
    * @brief Converts the world units to game units on the X axis.
    * 
    * @param worldX The X coordinate of the image to be converted.
    * 
    * @return An integer representing the X coordinate of the image in game units.
    */
    int WorldToScreenX(float worldX) const;

    /**
     * @brief Converts the world units to game units on the Y axis.
     *
     * @param worldX The Y coordinate of the image to be converted.
     *
     * @return An integer representing the Y coordinate of the image in game units.
     */
    int WorldToScreenY(float worldY) const;

    /**
    * @brief Returns the viewport width (scaled) of the camera/
    * 
    * @return An integer representing the viewport width of the camera in pixels.
    */
    int GetViewportWidth() const;

    /**
    * @brief Returns the viewport height (scaled) of the camera/
    *
    * @return An integer representing the viewport height of the camera in pixels.
    */
    int GetViewportHeight() const;

    /**
    * @brief Returns the X offset of the camera.
    * 
    * @return The X offset of the camera.
    */
    float GetXOffset() const;

    /**
    * @brief Returns the Y offset of the camera.
    *
    * @return The Y offset of the camera.
    */
    float GetYOffset() const;

    /**
    * @brief Returns the scale of the camera.
    * 
    * @return A float representing the current scale of the camera.
    */
    float GetScale() const;

    /**
    * @brief Sets the 'firstUpdate' flag to the value of the parameter.
    * 
    * @param value Value for the flag.
    */
    void SetFirstUpdate(bool value);
};

#endif // CAMERA_H_
