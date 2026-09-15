#pragma once

#ifndef GUARD_H_
#define GUARD_H_

#include <optional>

#include "Entity.h"
#include "Observer.h"
#include "GuardTileMover.h"
#include "TileInstance.h"
#include "GuardBehavior.h"
#include "Configuration.h"
#include "DialogueData.h"

class GuardState;
class Player;

struct SDL_Texture;

/**
* @brief Class for a guard enemy.
*/
class Guard : public Entity
{
private:
	static Player* player;	/// Static pointer to the player. NON-OWNING!
	static int guardCount;	/// The number of guards still existing.

	GuardTileMover tileMover;	/// Tile mover for movement.

	GuardAIData AIData;	/// Data for the guard AI.
	GuardState* currentState = nullptr;	/// Current state of the guard. Semi-owning (state dependent).

	VisionRange visionRange = Configuration::Get().guard.MakeVisionRange(Configuration::Get().guard.vision.patrol);	/// Vision range of the guard.

	float timeSinceLastPlayerSeen = 0.0f;	/// Time since last seen the player.
	
	float visionReductionElapsed = 0.0;	/// The elapsed time of vision decrease (in ms).
	VisionRange startVisionRange = { 0, 0, 0, 0, 0, 0 };	/// The starting vision range from which to decrease.

	float aggressionLevel = 0.0f;				/// Aggression level of the guard. Should be between 0 and 1.
	float aggressionReductionElapsed = 0.0f;	/// The elapsed time of aggression decrease (in ms).

	int alertSprite = -1;	/// The alert icon to display.
	uint64_t alertStartTime = 0;	/// The alert start time.
	uint64_t chaseStartTime = 0;	/// Start time of the chase.
	float alertIconElapsedTime = 0.0f;	/// Elapsed alert icon time.

	std::optional<DialogueData> caughtDialogueData;	/// Optional dialogue data for when the player is caught.

	bool showChaseAlertFlag = false;		/// Flag if the guard can show the chase alert.
	bool isReducingVisionFlag = false;		/// Flag if currently reducing vision.
	mutable bool showAlertFlag = false;		/// Flag if the guard can show an alert.
	bool isReducingAggressionFlag = false;	/// Flag if currently reducing aggression.

	// No copying allowed due to some states being uncopyable (singleton).
	Guard(const Guard&) = delete;
	Guard& operator=(const Guard&) = delete;

	// No move operator due to some members not being moveable by operator.
	Guard& operator=(Guard&&) = delete;

	/**
	* @brief Determines if the guard has line of sight to the player.
	* 
	* @return 'true' if the guard has line of sight to the player, 'false' if otherwise (vision obstructed).
	*/
	bool HasLineOfSightToPlayer() const;

	/**
	* @brief Determines if the player is in the vision cone of the guard.
	* 
	* @return 'true' if the player is in the vision cone of the guard, 'false' if otherwise.
	*/
	bool IsPlayerInVisionCone() const;

	/**
	* @brief Initialises the guard.
	* 
	* @param facingDirection The direction the guard should face when spawned in.
	*/
	void Init(Direction facingDirection = Direction::Right);

public:

	/**
	* @brief Constructor.
	* 
	* Initialises the guard with an idle and return behavior.
	* 
	* @param startX The starting X coordinate (tile index) of the guard.
	* @param startY The starting Y coordinate (tile index) of the guard.
	* @param facingDirection The direction the guard should face when spawned in.
	*/
	explicit Guard(int startX = 0, int startY = 0, Direction facingDirection = Direction::Right);

	/**
	* @brief Constructor.
	*
	* Initialises the guard with an idle and return behavior.
	*
	* @param spawnPoint The starting coordinates (tile indexes) of the guard.
	* @param facingDirection The direction the guard should face when spawned in.
	*/
	explicit Guard(const SDL_Point& spawnPoint = { 0, 0 }, Direction facingDirection = Direction::Right);

	/**
	* @brief Constructor.
	*
	* Initialises the guard with a patrol behavior.
	*
	* @param startX The starting X coordinate (tile index) of the guard.
	* @param startY The starting Y coordinate (tile index) of the guard.
	* @param patrolPoints Vector with the patrol points (tile indexes) of the guard.
	* @param facingDirection The direction the guard should face when spawned in.
	*/
	Guard(int startX, int startY, const std::vector<SDL_Point>& patrolPoints, Direction facingDirection = Direction::Right);

	/**
	* @brief Constructor.
	*
	* Initialises the guard with a patrol behavior.
	*
	* @param spawnPoint The starting coordinates (tile index) of the guard.
	* @param patrolPoints Vector with the patrol points (tile indexes) of the guard.
	* @param facingDirection The direction the guard should face when spawned in.
	*/
	Guard(const SDL_Point& spawnPoint, const std::vector<SDL_Point>& patrolPoints, Direction facingDirection = Direction::Right);

	/**
	* @brief Move constructor.
	*/
	Guard(Guard&& other) noexcept;

	/**
	* @brief Destructor.
	*/
	~Guard() override;

	/**
	* @brief Updates the guard enemy. 
	* 
	* @param deltaTime the delta time of the main SDL loop.
	*/
	void Update(double deltaTime) override;

	/**
	* @brief Determines if the guard can see the player or not.
	* 
	* @return true if the guard can see the player, false if otherwise.
	*/
	bool CanSeePlayer();

	/**
	* @brief Sets the guard to a new position.
	* 
	* @param x The X coordinate of the new position.
	* @param y The Y coordinate of the new position.
	*/
	void SetPosition(float x, float y) override;

	/**
	* @brief Sets the position of the guard.
	*
	* Snaps to the nearest tile.
	*
	* @param newPosition The new position of the guard.
	*/
	void SetPosition(const SDL_FPoint& newPosition) override;

	/**
	* @brief Sets the position of the guard in relation to tiles on the map.
	* 
	* Snaps to the nearest tile.
	* 
	* @param x The X coordinate of the tile.
	* @param y The Y coordinate of the tile.
	*/
	void SetPositionTile(int x, int y) override;

	/**
	* @brief Sets the position of the guard in relation to tiles on the map.
	* 
	* Snaps to the nearest tile.
	* 
	* @param newPosition The new position (tile indexes) of the guard.
	*/
	void SetPositionTile(const SDL_Point& newPosition) override;

	/**
	* @brief Returns the speed of the guard.
	* 
	* @return The speed of the guard.
	*/
	float GetSpeed() const override;

	/**
	* @brief Sets the speed of the guard.
	* 
	* @param speed The new speed of the guard.
	*/
	void SetSpeed(float speed);

	/**
	* @brief Returns the X coordinate of the guard.
	* 
	* @return The X coordinate of the guard.
	*/
	float GetX() const override;

	/**
	* @brief Returns the Y coordinate of the guard.
	* 
	* @return The Y coordinate of the guard.
	*/
	float GetY() const override;

	/**
	* @brief Returns the position of the guard.
	*
	* @return The position of the guard as an SDL_FPoint.
	*/
	SDL_FPoint GetPosition() const override;

	/**
	* @brief Returns the X coordinate of the tile the guard is on.
	*
	* @return The X coordinate of the tile the guard is on.
	*/
	int GetTileX() const override;

	/**
	* @brief Returns the Y coordinate of the tile the guard is on.
	* 
	* @return The Y coordinate of the tile the guard is on.
	*/
	int GetTileY() const override;

	/**
	* @brief Returns the tile position of the entity.
	*
	* @return The tile position of the entity as an SDL_FPoint.
	*/
	SDL_Point GetTilePosition() const override;

	/**
	* @brief Returns if the guard is moving or not.
	* 
	* @return 'true' if the guard is moving, 'false' if otherwise.
	*/
	bool IsMoving() const;

	/**
	* @brief Sets the direction provider for the guard.
	* 
	* @param provider The new direction provider.
	*/
	void SetDirectionProvider(const std::function<Direction()>& provider);

	/**
	* @brief Returns the current state of the guard.
	* 
	* @return Pointer to the current state;
	*/
	GuardState* GetCurrentState() const;

	/**
	* @brief Sets the current state to the new state.
	* 
	* Use the state manager to change the state, this only changes the state without any checks.
	* 
	* @param newState The state to switch to.
	*/
	void SetCurrentState(GuardState* newState);

	/**
	* @brief Returns the AI data of the guard.
	* 
	* @return The GuardAIData of the guard.
	*/
	const GuardAIData& GetAIData() const;

	/**
	* @brief Force sets the vision range of the guard.
	* 
	* Stops reducing vision.
	* 
	* @param range The new vision range of the guard.
	*/
	void SetVisionRange(const VisionRange& range);

	/**
	* @brief Starts to decrease the vision of the guard until the default patrol range.
	*/
	void StartVisionDecrease();

	/**
	* @brief Updates the vision of the guard.
	*
	* @param deltaTime The deltaTime of the main SDL loop.
	*/
	void UpdateVision(double deltaTime);

	/**
	* @brief Returns the aggression level of the guard.
	* 
	* @return The aggression level of the guard.
	*/
	float GetAggressionLevel() const;

	/**
	* @brief Force sets the aggression of the guard.
	* 
	* Stops reducing aggression.
	* 
	* @param newAggression The new value for the aggression.
	*/
	void SetAggression(float newAggression);

	/**
	* @brief Adds aggression based on distance.
	* 
	* Stops reducing aggression.
	* 
	* @param distance Distance to the target.
	* @param deltaTime The delta time of the main SDL loop.
	*/
	void AddAggressionBasedOnDistance(float distance, double deltaTime);

	/**
	* @brief Starts to decrease the aggression of the guard.
	*/
	void StartAggressionDecrease();

	/**
	* @brief Updates the aggression of the guard.
	* 
	* @param deltaTime The deltaTime of the main SDL loop.
	*/
	void UpdateAggression(double deltaTime);

	/**
	* @brief Sets the alert icon to the chase icon.
	*/
	void ShowChaseAlertIcon();

	/**
	* @brief Sets the alert icon to the search icon.
	*/
	void ShowSearchAlertIcon();

	/**
	* @brief Sets the chase start time.
	* 
	* @param time The start time of the chase.
	*/
	void SetChaseStartTime(uint64_t time);

	/**
	* @brief Returns the chase start time.
	* 
	* @return The chase start time.
	*/
	uint64_t GetChaseStartTime() const;

	/**
	* @brief Sets the showChaseAlert flag.
	* 
	* @param value The new boolean value of the flag.
	*/
	void SetChaseAlertFlag(bool value);

	/**
	* @brief Returns if the guard can show the chase alert.
	* 
	* @return 'true' if the guard can show the chase alert, 'false' otherwise.
	*/
	bool CanShowChaseAlert() const;

	/**
	* @brief Checks if any alert is active.
	* 
	* @return 'true' if any alert is active, 'false' otherwise.
	*/
	bool IsAlertActive() const;

	/**
	* @brief Sets the active alert flag.
	* 
	* @param value New value for the active alert flag.
	*/
	void SetAlertActive(bool value);

	/**
	* @brief Returns the time that the last active alert has started.
	* 
	* @return The time that the last active alert has started.
	*/
	uint64_t GetAlertStartTime() const;

	/**
	* @brief Returns the sprite index of the alert icon.
	* 
	* @return The index of the alert icon.
	*/
	int GetAlertTileSprite() const;

	/**
	* @brief Returns the time since the guard has last seen the player.
	* 
	* @return The time since the guard has lasta sen the player.
	*/
	float GetTimeSinceLastPlayerSeen() const;

	/**
	* @brief Sets the time since the guard has last seen the player.
	* 
	* @param newTime The new time since the guard has last seen the player.
	*/
	void SetTimeSinceLastPlayerSeen(float newTime);

	/**
	* @brief Adds to the time since the guard has last seen the player.
	* 
	* @param addTime The time to be added since the guard has last seen the player.
	*/
	void AddTimeSinceLastPlayerSeen(float addTime);

	/**
	* @brief Sets the dialogue data for when the player is caught.
	* 
	* @param data The dialogue data to be set for when the player is caught.
	*/
	void SetCaughtDialogueData(const DialogueData& data);

	/**
	* @brief Returns the dialogue data for when the player is caught.
	* 
	* @return The dialogue data for when the player is caught, or std::nullopt if not set.
	*/
	std::optional<DialogueData> GetCaughtDialogueData() const;
};

#endif // GUARD_H_