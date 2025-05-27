//
// Created by Cem on 13.05.2022.
//
#pragma once
#include "engine/renderer/Mesh.h"
#include <engine/renderer/components/CModel.h>
#include "engine/physics/components/collision/CSphereCollision.h"
#include "../AMyActor.h"

namespace game::world
{
    class ABumper : public AMyActor
    {
    public:
        ABumper(glm::mat4 transform, engine::world::World* world);

    protected:
        engine::renderer::components::model_handle_t meshComp;

        engine::physics::components::sphereCollision_handle_t collisionComp;
    };
}


