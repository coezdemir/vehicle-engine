//
// Created by CemOe on 20.11.2021.
//
#include "AGoal.h"
#include "../ACar.h"

game::world::AGoal::AGoal(const glm::mat4 transform, engine::world::World* const world,
                            const ACheckpoint* const precedingCheckpoint)
        : ACheckpoint(transform, world), lastCheckpoint(precedingCheckpoint)
{
    boxCollisionComp->event_onOverlap.subscribe(
            [this](const engine::physics::components::CollisionInfo collisionInfo) {
                if (auto* comp =
                        dynamic_cast<engine::physics::components::CRaycastVehicle*>
                            ((engine::components::CComponent*)collisionInfo.otherBody->userData))
                {
                    goalReached((ACar*) comp->getInstantiator());
                    checkPointReached((ACar*) comp->getInstantiator());
                }
            }
    );
}

void game::world::AGoal::goalReached(ACar* const car)
{
    if (car->getLastCheckpoint() == lastCheckpoint)
        event_goalReached.fire(car);
}