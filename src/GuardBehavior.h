#pragma once

#ifndef GUARDBEHAVIOR_H_
#define GUARDBEHAVIOR_H_

#include <SDL3/SDL_rect.h>
#include <vector>

class GuardPathStrategy;
class GuardPatrolStrategy;
class Guard;

/**
* @brief Enumeration of the different guard behaviors.
*/
enum class GuardBehaviorType : uint8_t
{
	IdleReturn,	/// Returns to designated point and idles.
	Patrol,		/// Patrols a designated sets of points.
	COUNT		/// Number of elements.
};

/**
* @brief Union data for each behavior.
*/
union GuardBehaviorData
{
	GuardPathStrategy* moveToPointStrategy;			/// Move strategy for the idle and return behavior.
	GuardPatrolStrategy* patrolStrategy = nullptr;	/// Patrol strategy for the patrol behavior.
};

/**
* @brief AI Data for a guard.
* 
* Doesn't use the typedef because of the methods (this will not pass in C anyhow).
*/
struct GuardAIData 
{
	GuardBehaviorData data;	/// The behavior data of the guard (union).
	GuardBehaviorType type;	/// The behavior type of the guard.

	/**
	* @brief Constructor. Makes an idle and return behavior.
	* 
	* @param targetX The X coordinate of the target point (tile index).
	* @param targetY The Y coordinate of the target point (tile index).
	*/
	GuardAIData(int targetX, int targetY);

	/**
	* @brief Constructor. Makes an idle and return behavior.
	*
	* @param targetPoint The target point (tile index) for the guard to move to.
	*/
	explicit GuardAIData(const SDL_Point& targetPoint);

	/**
	* @brief Constructor. Makes a patrol behabior.
	* 
	* @param patrolPoints The patrol points (tile indexes) of the guard.
	*/
	explicit GuardAIData(const std::vector<SDL_Point>& patrolPoints);

	/**
	* @brief Copy constructor.
	*/
	GuardAIData(const GuardAIData& other);

	/**
	* @brief Move constructor.
	*/
	GuardAIData(GuardAIData&& other) noexcept;

	/**
	* @brief Destructor.
	* 
	* Deletes the pointer in the union.
	*/
	~GuardAIData();

	/**
	* @brief Copy operator.
	*/
	GuardAIData& operator=(const GuardAIData& other);

	/**
	* @brief Move operator.
	*/
	GuardAIData& operator=(GuardAIData&& other) noexcept;
};

#endif // GUARDVEHAVIOR_H_