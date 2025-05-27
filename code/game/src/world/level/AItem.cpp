//
// Created by CemOe on 29.12.2020.
//
#include "AItem.h"
#include <engine/random/Random.h>

game::world::AItem::AItem(const glm::mat4 transform, engine::world::World* const world)
        : AMyActor(transform, world), bWasCollected(false), bIsRespawning(false)
{
    // Init mesh component
    meshComp = rootEntity.addComponent<engine::renderer::components::CModel>();
    engine::renderer::components::CModel::ConstructionInfo meshInfo;
    meshInfo.instantiator = this;
    meshInfo.path = "game/prism/prism.obj";
    meshInfo.bFlunctuateEmission = true;
    meshInfo.flunctuationInterval = 2.5f;
    meshComp->init(&meshInfo);

    sphereCollisionComp = rootEntity.addComponent<engine::physics::components::CSphereCollision>();
    engine::physics::components::CSphereCollision::ConstructionInfo collisionInfo;
    collisionInfo.instantiator = this;
    collisionInfo.sphereRadius = 1.f;
    collisionInfo.bIsDynamic = false;
    collisionInfo.bIsSimulationShape = false;
    collisionInfo.bShouldDeactivateAfterCollision = true;
    collisionInfo.timeActive = -1.f;
    collisionInfo.bUpdateLocation = false;
    collisionInfo.bActiveFromStart = true;
    sphereCollisionComp->init(&collisionInfo);
    sphereCollisionComp->event_onOverlap.subscribe(
            [this](const engine::physics::components::CollisionInfo collisionInfo) {
                if (auto* comp = dynamic_cast<engine::physics::components::CRaycastVehicle*>((engine::components::CComponent*) collisionInfo.otherBody->userData))
                    if (!bIsRespawning)
                        itemCollected((ACar*) comp->getInstantiator());
            }
    );

    collectedSound = rootEntity.addComponent<engine::audio::components::CAudio>();
    engine::audio::components::CAudio::ConstructionInfo audioConstruction;
    audioConstruction.instantiator = this;
    audioConstruction.pathToWav = "sound-effects/item-collected.wav";
    audioConstruction.volume = 6.f;
    collectedSound->init(&audioConstruction);

    transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* const traverser) {
        meshComp->visit(traverser);
        sphereCollisionComp->visit(traverser);
    });
}

void game::world::AItem::tick(const float deltaTime)
{
    AActor::tick(deltaTime);

    if (bWasCollected)
    {
        bWasCollected = false;

        bIsRespawning = true;

        float currentFPS = (1.f / deltaTime);
        respawnSteps = int(currentFPS * RESPAWN_TIME_SEC);

        return;
    }

    if (bIsRespawning)
    {
        if (--respawnSteps < 0)
        {
            bIsRespawning = false;

            meshComp->setVisibility(true);
        }
    }
}

void game::world::AItem::itemCollected(ACar* const car)
{
    collectedSound->play();

    // Randomly set item of the car
    car->setCurrentItem(static_cast<EItem>(engine::random::range(1, 4)));

    meshComp->setVisibility(false);

    bWasCollected = true;
}
