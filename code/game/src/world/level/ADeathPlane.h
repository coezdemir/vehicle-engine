//
// Created by CemOe on 28.12.2020.
//
#pragma once
#include <engine/physics/components/collision/CBoxCollision.h>
#include "../AMyActor.h"

namespace game::world
{
    class ADeathPlane final : public AMyActor
    {
    public:
        ADeathPlane(glm::mat4 transform, engine::world::World* world);

    private:
        static void killOverlappedActor(const physx::PxRigidActor* body);

        engine::physics::components::boxCollision_handle_t boxCollisionComp;
    };
}