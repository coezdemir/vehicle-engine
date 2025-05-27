//
// Created by Cem on 20.07.2022.
//

#include "engine/scene/components/CScale.h"
#include "engine/math/Math.h"

engine::scene::components::CScale::CScale(engine::components::id_t id, engine::entities::id_t owner)
    : CComponent(id, owner), t(0)
{

}

void engine::scene::components::CScale::init(const engine::components::ConstructionInfo* constructionInfo)
{
    CComponent::init(constructionInfo);

    // Check if we have the right construction info
    auto* scaleConstruction =
            dynamic_cast<const CScale::ConstructionInfo* const>(constructionInfo);
    assert(scaleConstruction &&
                   "Please use the correct ConstructionInfo: CTranslation::ConstructionInfo");

    bIsActive = scaleConstruction->bActiveFromStart;
    transformComp = scaleConstruction->transformComp;
    initialTransform = transformComp->getLocalTransform();
    scaleBetween = scaleConstruction->scaleBetween;
    speed = scaleConstruction->speed;
    bShouldReverse = scaleConstruction->bShouldReverse;
}

void engine::scene::components::CScale::tick(float deltaTime)
{
    CComponent::tick(deltaTime);

    if(!bIsActive)
        return;

    t += speed * deltaTime;

    if(t > 1)
    {
        if (bShouldReverse)
        {
            std::pair<glm::vec3, glm::vec3> temp = scaleBetween;
            scaleBetween.first = temp.second;
            scaleBetween.second = temp.first;
            t = 0;
        }
        else
        {
            deactivate();
        }
    }

    glm::vec3 lerpedScale = engine::math::lerp(scaleBetween.first, scaleBetween.second, t);

    glm::mat4 newScaledTransform = glm::scale(initialTransform, lerpedScale);
    transformComp->setLocalTransform(newScaledTransform);
}

void engine::scene::components::CScale::activate()
{
    bIsActive = true;
}

void engine::scene::components::CScale::deactivate()
{
    bIsActive = false;
}

void engine::scene::components::CScale::reset()
{
    t = 0;

    glm::vec3 lerpedScale = engine::math::lerp(scaleBetween.first, scaleBetween.second, t);

    glm::mat4 localTransform = transformComp->getLocalTransform();
    localTransform = glm::scale(localTransform, lerpedScale);
    transformComp->setLocalTransform(localTransform);
}
