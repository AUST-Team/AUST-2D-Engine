#pragma once

#ifndef ACTIVATIONEXPRESSION_H_
#define ACTIVATIONEXPRESSION_H_

#include "MapUpdateFlags.h"
#include "SpatialBuckets.h"

class Player;
class Trigger;
class Condition;

/**
* @brief An activator (logic) expression.
*/
class ActivationExpression
{
private:
    Condition* root = nullptr;  /// Root of the condition tree. Might or might not own it. Only deletes the owned parts.
                                /// Non-owned parts are owned by the ConditionRegistry.
    MapUpdateFlags updateFlags = MapUpdateFlags::None; /// Update flags of the condition.
    SpatialBucket spatialBucket = SpatialBucket::TileOnly;  /// The spatial bucket of the expression.

    // No copying due to polymorphic copying.
    ActivationExpression(const ActivationExpression&) = delete;
    ActivationExpression& operator=(const ActivationExpression&) = delete;

public:

    /**
    * @brief Constructor.
    * 
    * @param conditionRoot The roof of the condition tree.
    */
    explicit ActivationExpression(Condition* conditionRoot = nullptr);

    /**
    * @brief Move constructor.
    */
    ActivationExpression(ActivationExpression&& other) noexcept;

    /**
    * @brief Destructor.
    */
    ~ActivationExpression();

    /**
    * @brief Evaluates the expression.
    * 
    * @param trigger Pointer to the trigger being evaluated. NON OWNING!
    * @param player Pointer to the player. NON OWNING!
    * @param updateMask Update flags. A trigger will not fire if its update flag is not found.
    *
    * @return 'true' if expression is satisfied, 'false' if otherwise.
    */
    bool Evaluate(Trigger* trigger, Player* player, MapUpdateFlags updateMask) const;

    /**
    * @brief Returns the MapUpdateFlags of the expression.
    * 
    * @return The MapUpdateFlags of the expression.
    */
    MapUpdateFlags GetUpdateFlags() const;

    /**
    * @brief Returns the spatial bucket of the activation expression.
    * 
    * @return The spatial bucket of the activation expression.
    */
    SpatialBucket GetSpatialBucket() const;

    /**
    * @brief Move operator.
    */
    ActivationExpression& operator=(ActivationExpression&& other) noexcept;
};

#endif // ACTIVATIONEXPRESSION_H_


