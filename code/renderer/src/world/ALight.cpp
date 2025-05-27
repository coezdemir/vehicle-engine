//
// Created by CemOe on 18.10.2021.
//

#include "engine/renderer/world/ALight.h"

engine::renderer::world::ALight::ALight(const glm::mat4 transform, engine::world::World* const world) : AActor(world)
{
    transformComp = getRootEntity().addComponent<engine::scene::components::CTransform>();
    engine::scene::components::CTransform::ConstructionInfo transformInfo;
    transformInfo.instantiator = this;
    transformInfo.transform = transform;
    transformComp->init(&transformInfo);

    lightComponent = rootEntity.addComponent<engine::renderer::components::CLight>();
    engine::renderer::components::CLight::ConstructionInfo lightInfo =
            engine::renderer::components::CLight::ConstructionInfo();
    lightInfo.instantiator = this;
    lightComponent->init(&lightInfo);

    transformComp->event_init.subscribe([this](engine::scene::TreeTraverser* traverser){
        lightComponent->initialVisit(traverser);
    });

    transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* traverser){
        lightComponent->visit(traverser);
    });
}