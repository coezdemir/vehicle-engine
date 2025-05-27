//
// Created by Cem on 26.07.2022.
//

#include "ACube.h"

demo::world::ACube::ACube(glm::mat4 transform, engine::world::World* world) : AActor(world)
{
    // Init transform component
    transformComp = rootEntity.addComponent<engine::scene::components::CTransform>();
    engine::scene::components::CTransform::ConstructionInfo transformInfo;
    transformInfo.instantiator = this;
    transformInfo.transform = transform;
    transformComp->init(&transformInfo);

    // Init mesh component
    meshComp = rootEntity.addComponent<engine::renderer::components::CModel>();
    engine::renderer::components::CModel::ConstructionInfo meshInfo;
    meshInfo.instantiator = this;
    meshInfo.path = "cube.obj";
    meshComp->init(&meshInfo);

    collisionComp = rootEntity.addComponent<engine::physics::components::CBoxCollision>();
    engine::physics::components::CBoxCollision::ConstructionInfo collisionInfo;
    collisionInfo.instantiator = this;
    collisionInfo.boxHalfExtent = glm::vec3(1.f, 1.f, 1.f);
    collisionInfo.bIsDynamic = true;
    collisionInfo.bIsKinematic = false;
    collisionInfo.bIsSimulationShape = true;
    collisionInfo.mass = 10.f;
    collisionInfo.bShouldDeactivateAfterCollision = false;
    collisionInfo.timeActive = -1.f;
    collisionInfo.bActiveFromStart = true;
    collisionComp->init(&collisionInfo);

    transformComp->event_init.subscribe([this](engine::scene::TreeTraverser* traverser) {
        meshComp->initialVisit(traverser);
        collisionComp->initialVisit(traverser);
    });

    transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* traverser) {
        meshComp->visit(traverser);
        collisionComp->visit(traverser);
    });
}
