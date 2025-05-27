//
// Created by CemOe on 28.12.2020.
//
#include "ADeathPlane.h"

game::world::ADeathPlane::ADeathPlane(const glm::mat4 transform, engine::world::World* const world)
        : AMyActor(transform, world)
{
    // Init box component
    boxCollisionComp = rootEntity.addComponent<engine::physics::components::CBoxCollision>();
    engine::physics::components::CBoxCollision::ConstructionInfo collisionInfo;
    collisionInfo.instantiator = this;
    collisionInfo.boxHalfExtent = glm::vec3(5000.f, 50.f, 5000.f);
    collisionInfo.bIsDynamic = false;
    collisionInfo.bIsSimulationShape = false;
    collisionInfo.bShouldDeactivateAfterCollision = false;
    collisionInfo.timeActive = -1.f;
    collisionInfo.bUpdateLocation = false;
    collisionInfo.bActiveFromStart = true;
    boxCollisionComp->init(&collisionInfo);

    boxCollisionComp->event_onOverlap.subscribe(
            [](const engine::physics::components::CollisionInfo& collisionInfo) {
                killOverlappedActor(collisionInfo.otherBody);
            }
    );

    transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* const traverser) {
        boxCollisionComp->visit(traverser);
    });
}

void game::world::ADeathPlane::killOverlappedActor(const physx::PxRigidActor* const body)
{
    auto* comp = reinterpret_cast<engine::physics::components::CPhysicsComponent*>(body->userData);
    comp->getInstantiator()->destroy();
}
