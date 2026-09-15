#pragma once

#ifndef CONDITIONREGISTRY_H_
#define CONDITIONREGISTRY_H_

#include "ConditionKey.h"

class Condition;

/**
* @brief Registry for conditions. Only contains base conditions, no composite or logical ones (AND, OR, NOT, etc);
*/
class ConditionRegistry
{
private:
    std::unordered_map<ConditionKey, Condition*> conditionsByKey;   /// Map of [ConditionKey, Condition *]. OWNS THEM!

    // No copying due to polymorphic copying.
    ConditionRegistry(const ConditionRegistry&) = delete;
    ConditionRegistry& operator=(const ConditionRegistry&) = delete;

public:

    /**
    * @brief Constructor.
    * 
    * @param conditions Map of [ConditionKey, Condition *].
    */
    ConditionRegistry(const std::unordered_map<ConditionKey, Condition*>& conditions = {});

    /**
    * @brief Move constructor.
    */
    ConditionRegistry(ConditionRegistry&& other) noexcept;

    /**
    * @brief Destructor.
    */
    ~ConditionRegistry();

    /**
    * @brief Returns or creates and returns a condition from a key.
    * 
    * @param key Key of the condition.
    * 
    * @return Pointer to the condition.
    */
    Condition* GetOrCreateCondition(ConditionKey key);

    /**
    * @brief Move operator.
    */
    ConditionRegistry& operator=(ConditionRegistry&& other) noexcept;
};

#endif // CONDITIONREGISTRY_H_
