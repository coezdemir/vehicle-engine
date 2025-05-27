//
// Created by Cem on 26.07.2022.
//
#pragma once

#include <engine/world/AActor.h>
#include <engine/scene/components/CTransform.h>
#include <engine/renderer/components/CModel.h>
#include <engine/physics/components/collision/CBoxCollision.h>

namespace demo::world
{
    class ACube : public engine::world::AActor
    {
    public:
        ACube(glm::mat4 transform, engine::world::World* world);

    private:
        engine::scene::components::transform_handle_t transformComp;

        engine::renderer::components::model_handle_t meshComp;

        engine::physics::components::boxCollision_handle_t collisionComp;
    };
}