//
// Created by CemOe on 18.04.2021.
//

#include "engine/renderer/world/ACamera.h"

engine::renderer::world::ACamera::ACamera(glm::mat4 transform, engine::world::World* world, const std::function<glm::vec3()>& targetLocFunction)
    : AActor(world), targetLocFunction(targetLocFunction)
{
    transformComp = getRootEntity().addComponent<engine::scene::components::CTransform>();
    engine::scene::components::CTransform::ConstructionInfo transformInfo;
    transformInfo.instantiator = this;
    transformInfo.transform = transform;
    transformComp->init(&transformInfo);

    cameraComp = getRootEntity().addComponent<engine::renderer::components::CCamera>();
    engine::renderer::components::CCamera::ConstructionInfo constructionInfo =
            engine::renderer::components::CCamera::ConstructionInfo();
    constructionInfo.instantiator = this;
    constructionInfo.targetLoc = targetLocFunction;
    cameraComp->init(&constructionInfo);

    transformComp->event_init.subscribe([this](engine::scene::TreeTraverser* traverser){
        cameraComp->initialVisit(traverser);
    });

    transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* traverser){
        cameraComp->visit(traverser);
    });
}