//
// Created by Cem on 19.07.2022.
//
#pragma once
#include <engine/renderer/world/AMesh.h>

namespace game::world
{
    class AGuide : public engine::renderer::world::AMesh
    {
    public:
        AGuide(glm::mat4 transform, engine::world::World* world);

    protected:
        engine::scene::components::transform_handle_t transformComp2;

        engine::renderer::components::model_handle_t meshComp2;

        engine::scene::components::transform_handle_t transformComp3;

        engine::renderer::components::model_handle_t meshComp3;

        engine::scene::components::transform_handle_t transformComp4;

        engine::renderer::components::model_handle_t meshComp4;
    };
}

