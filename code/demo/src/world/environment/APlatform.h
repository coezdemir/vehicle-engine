//
// Created by Cem on 26.07.2022.
//
#pragma once
#include <engine/world/AActor.h>
#include <engine/scene/components/CTransform.h>
#include <engine/renderer/components/CModel.h>
#include <engine/physics/components/collision/CComplexCollision.h>

namespace demo::world
{
    class APlatform : public engine::world::AActor
    {
    public:
        APlatform(glm::mat4 transform, engine::world::World* world);

    private:
        engine::scene::components::transform_handle_t transformComp;

        engine::scene::components::transform_handle_t cubeScaling;

        engine::renderer::components::model_handle_t meshComp;

        engine::physics::components::complexCollision_handle_t collisionComp;

        std::vector<std::vector<glm::vec3>> vertices;

        std::vector<std::vector<unsigned int>> indices;
    };
}


