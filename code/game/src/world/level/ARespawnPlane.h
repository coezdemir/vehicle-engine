//
// Created by CemOe on 28.12.2020.
//
#pragma once
#include <engine/physics/components/collision/CBoxCollision.h>
#include "../AMyActor.h"

namespace game::world
{
    class ARespawnPlane final : public AMyActor
    {
    public:
        ARespawnPlane(glm::mat4 transform, engine::world::World* world);

    private:
        void respawnOverlappedCars(const physx::PxRigidActor* bodies);

        engine::physics::components::boxCollision_handle_t boxCollisionComp;
    };
}