//
// Created by CemOe on 20.11.2021.
//
#pragma once
#include <engine/renderer/Mesh.h>
#include <engine/renderer/components/CModel.h>
#include <engine/physics/components/collision/CBoxCollision.h>
#include "ACheckpoint.h"

namespace game::world
{
    class AGoal final : public ACheckpoint
    {
    public:
        /* Events */
        using goalReached_event_t = typename engine::callbacks::Event<AGoal, ACar* const>;

        AGoal(glm::mat4 transform, engine::world::World* world, const ACheckpoint* precedingCheckpoint);

        goalReached_event_t event_goalReached;

    private:
        void goalReached(ACar* car);

        const ACheckpoint* const lastCheckpoint;
    };
}
