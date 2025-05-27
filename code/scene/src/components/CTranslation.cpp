//
// Created by CemOe on 06.11.2021.
//

#include "engine/scene/components/CTranslation.h"
#include "engine/math/Math.h"

engine::scene::components::CTranslation::CTranslation(const engine::components::id_t id, const engine::entities::id_t owner)
    : CComponent(id, owner), t(0), bIsCoolingDown(false)
{

}

void engine::scene::components::CTranslation::init(const engine::components::ConstructionInfo* const constructionInfo)
{
    CComponent::init(constructionInfo);

    // Check if we have the right construction info
    auto* translationConstruction =
            dynamic_cast<const CTranslation::ConstructionInfo* const>(constructionInfo);
    assert(translationConstruction &&
        "Please use the correct ConstructionInfo: CTranslation::ConstructionInfo");

    transformComp = translationConstruction->transformComp;
    pointsToMoveBetween = translationConstruction->pointsToMoveBetween;
    speed = translationConstruction->speed / 100;
    bShouldReverse = translationConstruction->bShouldReverse;
    cooldownTimeSec = translationConstruction->cooldownTimeSec;
}

void engine::scene::components::CTranslation::reverse()
{
    std::pair<glm::vec3, glm::vec3> temp = pointsToMoveBetween;
    pointsToMoveBetween.first = temp.second;
    pointsToMoveBetween.second = temp.first;
}

void engine::scene::components::CTranslation::checkForCooldown(const float deltaTime)
{
    if(cooldownTimeSec > 0.f)
    {
        bIsCoolingDown = true;
        float currentFPS = (1.f / deltaTime);
        cooldownSteps = int(currentFPS * cooldownTimeSec);
    }
}

void engine::scene::components::CTranslation::tick(const float deltaTime)
{
    CComponent::tick(deltaTime);

    if(bIsCoolingDown)
    {
        bIsCoolingDown = --cooldownSteps > 0;
        return;
    }

    t += speed * deltaTime;

    if(t > 1)
    {
        if(bShouldReverse)
        {
            reverse();
            t = 0;
        }

        checkForCooldown(deltaTime);
    }

    glm::vec3 lerpedVector = engine::math::lerp(pointsToMoveBetween.first, pointsToMoveBetween.second, t);

    glm::mat4 localTransform = transformComp->getLocalTransform();
    localTransform[3] = glm::vec4(lerpedVector, 1.f);
    transformComp->setLocalTransform(localTransform);
}
