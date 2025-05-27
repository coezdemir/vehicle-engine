//
// Created by Cem on 04.04.2022.
//

#include "PhysicsPlayground.h"
#include "../world/level/AItem.h"
#include "../physics/MyCollisionCallback.h"

game::level::PhysicsPlayground::PhysicsPlayground(engine::world::World* world) : ALevel(world)
{
    glm::mat4 transform = glm::mat4(1.f);
    transform = glm::scale(transform, glm::vec3(100.f, 1.f ,100.f));
    transformComp->setLocalTransform(transform);

    // Init mesh component
    meshComp = rootEntity.addComponent<engine::renderer::components::CModel>();
    engine::renderer::components::CModel::ConstructionInfo meshInfo;
    meshInfo.instantiator = this;
    meshInfo.path = "Cube.obj";
    meshComp->init(&meshInfo);

    // Init mesh collision
    boxCollisionComp = rootEntity.addComponent<engine::physics::components::CBoxCollision>();
    engine::physics::components::CBoxCollision::ConstructionInfo collisionInfo;
    collisionInfo.instantiator = this;
    collisionInfo.bIsDynamic = false;
    collisionInfo.bIsSimulationShape = true;
    collisionInfo.bShouldDeactivateAfterCollision = false;
    collisionInfo.timeActive = -1.f;
    collisionInfo.bUpdateLocation = false;
    collisionInfo.bActiveFromStart = true;
    collisionInfo.boxHalfExtent = glm::vec3(1.f, 1.f, 1.f);
    PxFilterData qryFilterData;
    qryFilterData.word3 = static_cast<PxU32>(engine::physics::vehicle::DRIVABLE_SURFACE);
    collisionInfo.qryFilterData = qryFilterData;
    boxCollisionComp->init(&collisionInfo);
    boxCollisionComp->event_onRigidBodyCreated.subscribe([](const physx::PxRigidActor* rigidbody){
        game::physics::collision::FilterGroup::setupFiltering(rigidbody,
                                                              engine::physics::vehicle::COLLISION_FLAG_GROUND,
                                                              engine::physics::vehicle::COLLISION_FLAG_GROUND_AGAINST);
    });

    transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* traverser) {
        meshComp->visit(traverser);
        boxCollisionComp->visit(traverser);
    });
}

void game::level::PhysicsPlayground::loadLevel(game::gamemode::Gamemode* gamemode)
{
    // Light
    glm::mat4 transform = glm::mat4(1.f);
    lightActor = world->createActor<engine::renderer::world::ALight>(transform, world);
    actors.push_back(lightActor);

    transform = glm::mat4(1.f);
    transform = glm::translate(transform, glm::vec3(0.f, 2.f, 20.f));
    actors.push_back(world->createActor<game::world::AItem>(transform, world));
}
