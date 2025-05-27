//
// Created by Cem on 15.05.2022.
//

#include "ARotatingBumper.h"
#include "../../physics/MyCollisionCallback.h"

game::world::ARotatingBumper::ARotatingBumper(const glm::mat4 transform, const glm::mat4 offset, engine::world::World* const world,
                                               engine::scene::components::CRotation::ConstructionInfo* rotationInfo)
                                                        : ABumper(transform, world)
{
    collisionComp = rootEntity.addComponent<engine::physics::components::CSphereCollision>();
    engine::physics::components::CSphereCollision::ConstructionInfo collisionInfo;
    collisionInfo.instantiator = this;
    collisionInfo.bIsDynamic = true;
    collisionInfo.bIsKinematic = true;
    collisionInfo.bIsSimulationShape = true;
    collisionInfo.bShouldDeactivateAfterCollision = false;
    collisionInfo.timeActive = -1.f;
    collisionInfo.bUpdateLocation = false;
    collisionInfo.sphereRadius = 2.5f;
    collisionInfo.bActiveFromStart = true;
    collisionComp->init(&collisionInfo);
    collisionComp->event_onRigidBodyCreated.subscribe([](const physx::PxRigidActor* rigidbody) {
        game::physics::collision::FilterGroup::setupFiltering(rigidbody,
                                                              game::physics::collision::FilterGroup::eOBSTACLE,
                                                              game::physics::collision::FilterGroup::eVEHICLE);
    });

    transformOffset = rootEntity.addComponent<engine::scene::components::CTransform>();
    engine::scene::components::CTransform::ConstructionInfo transformConstruction;
    transformConstruction.instantiator = this;
    transformConstruction.transform = offset;
    transformOffset->init(&transformConstruction);
    transformComp->addChild(transformOffset->getId());

    rotationComp = rootEntity.addComponent< engine::scene::components::CRotation>();
    rotationInfo->instantiator = this;
    rotationInfo->transformComp = transformComp;
    rotationComp->init(rotationInfo);

    // Detach mesh and collision from transformComp
    transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* const traverser){});

    transformOffset->event_visited.subscribe([this](engine::scene::TreeTraverser* const traverser){
        meshComp->visit(traverser);
        collisionComp->visit(traverser);
    });
}

void game::world::ARotatingBumper::tick(const float deltaTime)
{
    game::world::ABumper::tick(deltaTime);

    rotationComp->tick(deltaTime);
}
