//
// Created by CemOe on 08.12.2021.
//

#include "engine/scene/components/CStepRotation.h"

engine::scene::components::CStepRotation::CStepRotation(engine::components::id_t id, engine::entities::id_t owner)
        : CComponent(id, owner)
{

}

void engine::scene::components::CStepRotation::init(const engine::components::ConstructionInfo* const constructionInfo)
{
    CComponent::init(constructionInfo);

    // Check if we have the right construction info
    auto* construction =
            dynamic_cast<const CStepRotation::ConstructionInfo* const>(constructionInfo);
    assert(construction &&
                   "Please use the correct ConstructionInfo: CTranslation::ConstructionInfo");

    transformComp = construction->transformComp;
    axis = glm::normalize(construction->axis);
    goalValue = construction->goalValue;
    currentGoalValue = glm::abs(goalValue);
    speed = construction->speed;
    bShouldReverse = construction->bShouldReverse;
    direction = goalValue > 0 ? 1 : -1;
    cooldownTimeSec = construction->cooldownTimeSec;
    bIsCoolingDown = false;
    bShouldCooldown = cooldownTimeSec > 0.f;
}

void engine::scene::components::CStepRotation::tick(const float deltaTime)
{
    CComponent::tick(deltaTime);

    if(bIsCoolingDown)
    {
        bIsCoolingDown = --cooldownSteps > 0;
        return;
    }

    float currentValue = (speed * deltaTime * direction);
    transformComp->rotate(glm::radians(currentValue), axis);

    currentGoalValue -= glm::abs(currentValue);

    if(currentGoalValue < 0)
    {
        if(bShouldReverse)
            direction *= -1;

        if(bShouldCooldown )
        {
            bIsCoolingDown = true;

            float currentFPS = (1.f / deltaTime);
            cooldownSteps = int(currentFPS * cooldownTimeSec);
        }

        // Reset currentGoalValue
        currentGoalValue = glm::abs(goalValue);
    }
}
