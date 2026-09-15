#pragma once

#ifndef DISTANCES_H_
#define DISTANCES_H_

#include <cmath>
#include <SDL3/SDL_rect.h>

/**
* @brief Namespace for functions related to calculating distances.
*/
namespace Distances
{
	/**
	* @brief Calculates the euclidian distance between two points.
	*
	* @param startX The X coordinate of the starting position.
	* @param startY The Y coordinate of the starting position.
	* @param endX The X coordinate of the end position.
	* @param endY The Y coordinate of the end position.
	* 
	* @return The Euclidian distance between the two points.
	*/
	inline float GetEuclidianDistanceTo(float startX, float startY, float endX, float endY)
	{
		return std::hypot(startX - endX, startY - endY);
	}

	/**
	* @brief Calculates the euclidian distance between two points.
	*
	* @param start The starting point coordinates.
	* @param end The end point coordinates.
	* 
	* @return The Euclidian distance between the two points.
	*/
	inline float GetEuclidianDistanceTo(const SDL_FPoint& start, const SDL_FPoint& end)
	{
		return GetEuclidianDistanceTo(start.x, start.y, end.x, end.y);
	}

	/**
	* @brief Calculates the Manhattan distance between two points.
	*
	* @param startX The X coordinate of the starting position.
	* @param startY The Y coordinate of the starting position.
	* @param endX The X coordinate of the end position.
	* @param endY The Y coordinate of the end position.
	* 
	* @return The Manhattan distance between the two points.
	*/
	inline int GetManhattanDistanceTo(int startX, int startY, int endX, int endY)
	{
		const int dx = std::abs(startX - endX);
		const int dy = std::abs(startY - endY);
		return dx + dy;
	}

	/**
	* @brief Calculates the Manhattan distance between two points.
	*
	* @param start The starting point coordinates.
	* @param end The end point coordinates.
	* 
	* @return The Manhattan distance between the two points.
	*/
	inline int GetManhattanDistanceTo(const SDL_Point& start, const SDL_Point& end)
	{
		return GetManhattanDistanceTo(start.x, start.y, end.x, end.y);
	}

	/**
	* @brief Calculates the Chebyshev distance between two points.
	*
	* @param startX The X coordinate of the starting position.
	* @param startY The Y coordinate of the starting position.
	* @param endX The X coordinate of the end position.
	* @param endY The Y coordinate of the end position.
	* 
	* @return The Chebyshev distance between the two points.
	*/
	inline int GetChebyshevDistanceTo(int startX, int startY, int endX, int endY)
	{
		const int dx = std::abs(startX - endX);
		const int dy = std::abs(startY - endY);
		return std::max(dx, dy);
	}

	/**
	* @brief Calculates the Chebyshev distance between two points.
	*
	* @param start The starting point coordinates.
	* @param end The end point coordinates.
	* 
	* @return The Chebyshev distance between the two points.
	*/
	inline int GetChebyshevDistanceTo(const SDL_Point& start, const SDL_Point& end)
	{
		return GetChebyshevDistanceTo(start.x, start.y, end.x, end.y);
	}
}

#endif // DISTANCES_H_