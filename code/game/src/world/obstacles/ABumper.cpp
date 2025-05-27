//
// Created by Cem on 13.05.2022.
//
#include "ABumper.h"
#include "../../physics/MyCollisionCallback.h"

game::world::ABumper::ABumper(const glm::mat4 transform, engine::world::World* const world)
        : AMyActor(transform, world)
{
    meshComp = rootEntity.addComponent<engine::renderer::components::CModel>();
    engine::renderer::components::CModel::ConstructionInfo meshInfo;
    meshInfo.instantiator = this;
    meshInfo.path = "game/obstacles/bumper/Bumper.obj";
    meshComp->init(&meshInfo);

    collisionComp = rootEntity.addComponent<engine::physics::components::CSphereCollision>();
    engine::physics::components::CSphereCollision::ConstructionInfo collisionInfo;
    collisionInfo.instantiator = this;
    collisionInfo.bIsDynamic = false;
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

    transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* const traverser){
        meshComp->visit(traverser);
        collisionComp->visit(traverser);
    });
}

