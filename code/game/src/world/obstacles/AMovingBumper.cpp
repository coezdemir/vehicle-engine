//
// Created by Cem on 15.05.2022.
//

#include "AMovingBumper.h"
#include "../../physics/MyCollisionCallback.h"

game::world::AMovingBumper::AMovingBumper(const glm::mat4 transform, engine::world::World* const world,
                                          engine::scene::components::CTranslation::ConstructionInfo* translationInfo)
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

    translationComp = rootEntity.addComponent<engine::scene::components::CTranslation>();
    translationInfo->instantiator = this;
    translationInfo->transformComp = transformComp;
    translationComp->init(translationInfo);
}

void game::world::AMovingBumper::tick(const float deltaTime)
{
    AActor::tick(deltaTime);

    translationComp->tick(deltaTime);
}
