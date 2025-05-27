//
// Created by CemOe on 19.11.2021.
//

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <engine/math/Math.h>
#include "ACheckpoint.h"
#include "../ACar.h"

game::world::ACheckpoint::ACheckpoint(const glm::mat4 transform, engine::world::World* const world)
        : AMyActor(transform, world)
{
    boxCollisionComp = rootEntity.addComponent<engine::physics::components::CBoxCollision>();
    engine::physics::components::CBoxCollision::ConstructionInfo collisionInfo;
    collisionInfo.instantiator = this;
    collisionInfo.boxHalfExtent = glm::vec3(.5f, .5f, .5f);
    collisionInfo.bIsDynamic = false;
    collisionInfo.bIsSimulationShape = false;
    collisionInfo.bShouldDeactivateAfterCollision = false;
    collisionInfo.timeActive = -1.f;
    collisionInfo.bUpdateLocation = false;
    collisionInfo.bActiveFromStart = true;
    boxCollisionComp->init(&collisionInfo);

    boxCollisionComp->event_onOverlap.subscribe(
            [this](const engine::physics::components::CollisionInfo collisionInfo) {
                if (auto* comp = dynamic_cast<engine::physics::components::CRaycastVehicle*>((engine::components::CComponent*) collisionInfo.otherBody->userData))
                    checkPointReached((ACar*) comp->getInstantiator());
            }
    );

    transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* const traverser) {
        boxCollisionComp->visit(traverser);
    });
}

glm::mat4 game::world::ACheckpoint::getRespawnTransform(const unsigned int id) const
{
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::mat4 checkpointTransform = transformComp->getTransform();
    engine::math::decomposeTransform(checkpointTransform, translation, rotation, scale);

    glm::mat4 respawnTransform = glm::mat4(1.f);
    respawnTransform = glm::translate(respawnTransform, translation);
    respawnTransform = glm::translate(respawnTransform, glm::vec3(0.f, 4.f, 0.f));
    respawnTransform = respawnTransform * toMat4(rotation);

    switch (id)
    {
        case 0:
            respawnTransform = glm::translate(respawnTransform, glm::vec3(20.f, 0.f, 0.f));
            break;
        case 1:
            respawnTransform = glm::translate(respawnTransform, glm::vec3(10.f, 0.f, 0.f));
            break;
        case 2:
            respawnTransform = glm::translate(respawnTransform, glm::vec3(-10.f, 0.f, 0.f));
            break;
        case 3:
            respawnTransform = glm::translate(respawnTransform, glm::vec3(-20.f, 0.f, 0.f));
            break;
    }


    return respawnTransform;
}

void game::world::ACheckpoint::checkPointReached(ACar* const car)
{
    car->setCheckpoint(this);
}