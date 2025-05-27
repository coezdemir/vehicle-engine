//
// Created by CemOe on 24.01.2022.
//

#include "engine/renderer/world/ACinematicCamera.h"

engine::renderer::world::ACinematicCamera::ACinematicCamera(const glm::mat4 transform, const glm::mat4 cameraOffset,
                                                    engine::world::World* const world) : AActor(world)
{
    transformComp = getRootEntity().addComponent<engine::scene::components::CTransform>();
    engine::scene::components::CTransform::ConstructionInfo transformInfo;
    transformInfo.instantiator = this;
    transformInfo.transform = transform;
    transformComp->init(&transformInfo);

    // Init transform component
    transformOffset = getRootEntity().addComponent<engine::scene::components::CTransform>();
    transformInfo.transform = cameraOffset;
    transformOffset->init(&transformInfo);
    transformComp->addChild(transformOffset->getId());

    cameraComp = getRootEntity().addComponent<engine::renderer::components::CCamera>();
    engine::renderer::components::CCamera::ConstructionInfo constructionInfo =
            engine::renderer::components::CCamera::ConstructionInfo();
    constructionInfo.instantiator = this;
    constructionInfo.targetLoc = [this](){return transformComp->getTransform()[3];};
    constructionInfo.cameraType = engine::renderer::components::CT_PERSPECTIVE;
    constructionInfo.bIsCinematic = true;
    cameraComp->init(&constructionInfo);

    rotationComponent = rootEntity.addComponent<engine::scene::components::CRotation>();
    engine::scene::components::CRotation::ConstructionInfo rotationInfo;
    rotationInfo.instantiator = this;
    rotationInfo.transformComp = transformComp;
    rotationInfo.axis = glm::vec3(0.f, 1.f, 0.f);
    rotationInfo.stepSize = 0.25f;
    rotationComponent->init(&rotationInfo);

    transformOffset->event_init.subscribe([this](engine::scene::TreeTraverser* traverser){
        cameraComp->initialVisit(traverser);
    });

    transformOffset->event_visited.subscribe([this](engine::scene::TreeTraverser* traverser){
        cameraComp->visit(traverser);
    });
}

void engine::renderer::world::ACinematicCamera::tick(const float deltaTime)
{
    AActor::tick(deltaTime);

    rotationComponent->tick(deltaTime);
}
