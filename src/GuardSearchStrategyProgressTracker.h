#pragma once

#ifndef GUARDSEARCHSTRATEGYPROGRESSTRACKER_H_
#define GUARDSEARCHSTRATEGYPROGRESSTRACKER_H_

// These names are getting long.

/**
* @brief Tracks the guard search strategy and sets a flag once done.
*/
class GuardSearchStrategyProgressTracker
{
public:
	
	/**
	* @brief Default destructor.
	*/
	virtual ~GuardSearchStrategyProgressTracker() = default;

	/**
	* @brief Returns the status of the search strategy.
	* 
	* @return 'true' if the strategy has finished, 'false' if otherwise. Default false.
	*/
	virtual bool IsSearchStrategyDone() const = 0;
};

#endif // GUARDSEARCHSTRATEGYPROGRESSTRACKER_H_
