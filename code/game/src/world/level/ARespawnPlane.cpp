//
// Created by CemOe on 28.12.2020.
//
#include "ARespawnPlane.h"
#include "../ACar.h"

game::world::ARespawnPlane::ARespawnPlane(const glm::mat4 transform, engine::world::World* const world)
    : AMyActor(transform, world)
{
    // Init box component
    boxCollisionComp = rootEntity.addComponent<engine::physics::components::CBoxCollision>();
    engine::physics::components::CBoxCollision::ConstructionInfo collisionInfo;
    collisionInfo.instantiator = this;
    collisionInfo.boxHalfExtent = glm::vec3(0.5f, 0.5f, 0.5f);
    collisionInfo.bIsSimulationShape = false;
    collisionInfo.bIsDynamic = false;
    collisionInfo.bShouldDeactivateAfterCollision = false;
    collisionInfo.timeActive = -1.f;
    collisionInfo.bUpdateLocation = false;
    collisionInfo.bActiveFromStart = true;
    boxCollisionComp->init(&collisionInfo);

    boxCollisionComp->event_onOverlap.subscribe(
            [this](const engine::physics::components::CollisionInfo& collisionInfo) {
                respawnOverlappedCars(collisionInfo.otherBody);
            }
    );

    transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* const traverser) {
        boxCollisionComp->visit(traverser);
    });
}

void game::world::ARespawnPlane::respawnOverlappedCars(const physx::PxRigidActor* const body)
{
    auto* comp = reinterpret_cast<engine::physics::components::CPhysicsComponent*>(body->userData);

    if(auto* car = dynamic_cast<ACar*>(comp->getInstantiator()))
        car->respawn();
}