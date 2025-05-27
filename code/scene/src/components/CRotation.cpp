//
// Created by CemOe on 06.11.2021.
//

#include "engine/scene/components/CRotation.h"
#include "engine/scene/TreeTraverser.h"

engine::scene::components::CRotation::CRotation(engine::components::id_t id, engine::entities::id_t owner)
    : CComponent(id, owner)
{

}

void engine::scene::components::CRotation::init(const engine::components::ConstructionInfo* constructionInfo)
{
    CComponent::init(constructionInfo);

    // Check if we have the right construction info
    auto* rotateConstruction =
            dynamic_cast<const CRotation::ConstructionInfo*>(constructionInfo);
    assert(rotateConstruction && "Please use the correct ConstructionInfo: CRotation::ConstructionInfo");

    transformComp = rotateConstruction->transformComp;
    axis = rotateConstruction->axis;
    stepSize = rotateConstruction->stepSize;
}

void engine::scene::components::CRotation::tick(const float deltaTime)
{
    CComponent::tick(deltaTime);

    transformComp->rotate(stepSize * deltaTime, axis);
}

void engine::scene::components::CRotation::setStepSize(const float stepSize)
{
    CRotation::stepSize = stepSize;
}
