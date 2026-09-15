#pragma once

#ifndef OBSERVER_H_
#define OBSERVER_H_

class Entity;

/**
* @brief Interface for the Observer pattern.
*/
class Observer
{
public:
	/**
	* Default destructor.
	*/
	virtual ~Observer() = default;

	/**
	* @brief Updates the object using a given Entity.
	* 
	* @param deltaTime The delta time from the main SDL loop.
	* @param subject The subject whose attributes will be used.
	*/
	virtual void Update(double deltaTime, const Entity& subject) = 0;
};

#endif // OBSERVER_H_
