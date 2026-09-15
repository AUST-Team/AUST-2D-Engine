#pragma once

#ifndef COMPOSITECONDITION_H_
#define COMPOSITECONDITION_H_

#include <vector>

#include "Condition.h"

enum class CompositeType : uint8_t
{ 
    AND,    /// AND between conditions.
    OR,     /// OR between conditions.
    NOT,    /// NOT condition. Only does a NOT to the first element of the vector!
    COUNT   /// Number of elements.
};

/**
* @brief Composite for conditions.
* 
* Unshareable.
*/
class CompositeCondition : public Condition
{
private:
    std::vector<Condition *> conditions;    /// Vector of conditions. Might or might not own then.
    CompositeType type;     /// Type of composite.

    // No copying due to polymorphic copying.
    CompositeCondition(const CompositeCondition&) = delete;
    CompositeCondition& operator=(const CompositeCondition&) = delete;

public:

    /**
    * @brief Constructor.
    * 
    * @param t Type of composition of the condition.
    * @param conditions Vector of conditions.
    */
    CompositeCondition(CompositeType t, const std::vector<Condition *>& conditions = {});

    /**
    * @brief Constructor.
    *
    * @param t Type of composition of the condition.
    * @param conditions Temporary vector of conditions.
    */
    CompositeCondition(CompositeType t, std::vector<Condition*>&& conditions = {});

    /**
    * @brief Move constructor.
    */
    CompositeCondition(CompositeCondition&& other) noexcept;

    /**
    * @brief Destructor.
    */
    ~CompositeCondition() override;

    /**
    * @brief Adds a condition to the composite.
    * 
    * @param cond Pointer to the condition to add. Might or might not own it.
    */
    void AddCondition(Condition* cond);

    /**
    * @brief Returns the children of the composite.
    * 
    * @return Reference to the vector of children.
    */
    std::vector<Condition*> GetConditions() const;

    /**
    * @brief Tests whether the composite condition is satisfied.
    *
    * @param trigger Pointer to the trigger being evaluated. NON OWNING!
    * @param player Pointer to the player. NON OWNING!
    * @param updateMask Update flags. A condition will not be satisfied if its update flag is not found.
    *
    * @return 'true' if composite condition is satisfied, 'false' if otherwise.
    */
    bool IsSatisfied(Trigger* trigger, Player* player, MapUpdateFlags updateMask) const override;

    /**
    * @brief Returns which map update flags the composite condition depends on.
    *
    * @return All the update flags of the base conditions.
    */
    MapUpdateFlags GetUpdateFlags() const override;

    /**
    * @brief Checks if the condition is shareable or not.
    *
    * @return 'false', composite condtion is not shareable.
    */
    bool IsShared() const override;

    /**
    * @brief Returns the spatial radius of the composite condition.
    *
    * @return The max spatial radius of the composite condition's children.
    */
    uint64_t GetSpatialRadius() const override;

    /**
    * @brief Move operator.
    */
    CompositeCondition& operator=(CompositeCondition&& other) noexcept;
};

#endif // COMPOSITECONDITION_H_