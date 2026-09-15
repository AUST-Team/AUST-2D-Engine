#pragma once

#ifndef ENTITY_H_
#define ENTITY_H_

#include <SDL3/SDL_rect.h>
#include <string>

#include "Size.h"

/**
* @brief Abstract class of a game entity.
*/
class Entity
{
protected:
    Size dimensions = { 0, 0 };    /// Size of the entity.
    int sprite = 0;     /// Sprite of the entity.

public:

    /**
    * @brief Constructor.
    */
    Entity();

    /**
    * @brief Default destructor.
    */
    virtual ~Entity() = default;

    /**
    * @brief Updates the entity.
    * 
    * @param deltaTime The delta time from the game loop.
    */
    virtual void Update(double deltaTime) = 0;

    /**
    * @brief Sets the position of the entity.
    *
    * @param x The X coordinate of the new position.
    * @param y The Y coordinate of the new position.
    */
    virtual void SetPosition(float x, float y) = 0;

    /**
    * @brief Sets the position of the entity.
    *
    * @param newPosition The new position of the entity.
    */
    virtual void SetPosition(const SDL_FPoint& newPosition) = 0;

    /**
    * @brief Sets the position of the entity in relation to tiles on the map.
    *
    * @param x The X coordinate of the tile.
    * @param y The Y coordinate of the tile.
    */
    virtual void SetPositionTile(int x, int y) = 0;

    /**
    * @brief Sets the position of the entity in relation to tiles on the map.
    *
    * @param newPosition The new position (tile indexes) of the entity.
    */
    virtual void SetPositionTile(const SDL_Point& newPosition) = 0;

    /**
    * @brief Returns the X coordinate of the entity as a float.
    * 
    * @return The X coordinate of the entity.
    */
    virtual float GetX() const = 0;

    /**
    * @brief Returns the Y coordinate of the entity as a float.
    * 
    * @return The Y coordinate of the entity.
    */
    virtual float GetY() const = 0;

    /**
    * @brief Returns the position of the entity.
    * 
    * @return The position of the entity as an SDL_FPoint.
    */
    virtual SDL_FPoint GetPosition() const;

    /**
    * @brief Returns the X coordinate of the tile the entity is on.
    *
    * @return The X coordinate of the tile the entity is on.
    */
    virtual int GetTileX() const;

    /**
    * @brief Returns the Y coordinate of the tile the entity is on.
    *
    * @return The Y coordinate of the tile the entity is on.
    */
    virtual int GetTileY() const;

    /**
    * @brief Returns the tile position of the entity.
    *
    * @return The tile position of the entity as an SDL_FPoint.
    */
    virtual SDL_Point GetTilePosition() const;

    /**
    * @brief Returns the width of the entity as an integer.
    * 
    * @return The default width of an entity.
    */
    virtual int GetWidth() const;

    /**
    * @brief Returns the height of the entity as an integer.
    *
    * @return The default height of an entity.
    */
    virtual int GetHeight() const;

    /**
    * @brief Returns the dimensions of the entity.
    * 
    * @return The dimensions of the entity.
    */
    virtual Size GetSize() const;

    /**
     * @brief Returns the speed of the entity as a float.
     * 
     * The speed is usually managed by a tile mover, treat this as a fallback.
     *
     * @return The default speed of an entity.
     */
    virtual float GetSpeed() const;

    /**
    * @brief Sets the sprite of the entity.
    * 
    * @param newSprite ID of the new sprite of the entity.
    */
    virtual void SetSprite(int newSprite);

    /**
    * @brief Sets the sprite of the entity.
    *
    * @param newSprite Name of the new sprite of the entity.
    */
    virtual void SetSprite(const std::string& newSprite);

    /**
    * @brief Returns the sprite of the entity.
    * 
    * @return The index of the sprite of the entity.
    */
    virtual int GetSprite() const;
};

#endif // ENTITY_H_
