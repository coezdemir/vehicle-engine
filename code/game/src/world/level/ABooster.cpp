//
// Created by Cem on 21.07.2022.
//
#include <engine/physics/components/vehicles/CRaycastVehicle.h>
#include "ABooster.h"

game::world::ABooster::ABooster(glm::mat4 transform, engine::world::World* world) : AMyActor(transform, world)
{
    meshComp = rootEntity.addComponent<engine::renderer::components::CModel>();
    engine::renderer::components::CModel::ConstructionInfo meshInfo;
    meshInfo.instantiator = this;
    meshInfo.path = "game/booster/booster.obj";
    meshInfo.bFlunctuateEmission = false;
    meshComp->init(&meshInfo);

    glm::mat4 collisionOffset = glm::mat4(1.f);
    collisionOffset = glm::translate(collisionOffset, glm::vec3(0.f, 1.f, 0.f));
    collisionTransformComp = rootEntity.addComponent<engine::scene::components::CTransform>();
    engine::scene::components::CTransform::ConstructionInfo transformInfo;
    transformInfo.instantiator = this;
    transformInfo.transform = collisionOffset;
    collisionTransformComp->init(&transformInfo);
    transformComp->addChild(collisionTransformComp->getId());

    // Init box component
    boxCollisionComp = rootEntity.addComponent<engine::physics::components::CBoxCollision>();
    engine::physics::components::CBoxCollision::ConstructionInfo collisionInfo;
    collisionInfo.instantiator = this;
    collisionInfo.boxHalfExtent = glm::vec3(4.f, 0.5f, 4.f);
    collisionInfo.bIsDynamic = false;
    collisionInfo.bIsSimulationShape = false;
    collisionInfo.bShouldDeactivateAfterCollision = false;
    collisionInfo.timeActive = -1.f;
    collisionInfo.bUpdateLocation = false;
    collisionInfo.bActiveFromStart = true;
    boxCollisionComp->init(&collisionInfo);

    boxCollisionComp->event_onOverlap.subscribe([](const engine::physics::components::CollisionInfo& collisionInfo) {
                applyBoost(collisionInfo.otherBody);
    });

    transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* const traverser) {
        meshComp->visit(traverser);
    });

    collisionTransformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* const traverser) {
        boxCollisionComp->visit(traverser);
    });
}

void game::world::ABooster::applyBoost(const physx::PxRigidActor* body)
{
    if (auto* vehicle =
            dynamic_cast<engine::physics::components::CRaycastVehicle*>((engine::components::CComponent*) body->userData))
    {
        vehicle->boost(BOOST_VALUE);
    }
}
