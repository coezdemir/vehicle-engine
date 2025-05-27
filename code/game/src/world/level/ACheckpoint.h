//
// Created by CemOe on 19.11.2021.
//
#pragma once
#include <engine/renderer/Mesh.h>
#include <engine/renderer/components/CModel.h>
#include <engine/physics/components/collision/CBoxCollision.h>
#include "../AMyActor.h"

namespace game::world
{
    class ACar;

    class ACheckpoint : public AMyActor
    {
    public:
        ACheckpoint(glm::mat4 transform, engine::world::World* world);

        [[nodiscard]] glm::mat4 getRespawnTransform(const unsigned int id) const;

    protected:
        void checkPointReached(ACar* car);

        engine::physics::components::boxCollision_handle_t boxCollisionComp;
    };
}
