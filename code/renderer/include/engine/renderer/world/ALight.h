//
// Created by CemOe on 18.10.2021.
//
#pragma once
#include "engine/renderer/components/CLight.h"
#include "../../../../../game/src/world/AMyActor.h"

namespace engine::renderer::world
{
class ALight : public engine::world::AActor
    {
    public:
        ALight(glm::mat4 transform, engine::world::World* world);

    private:
        engine::scene::components::transform_handle_t transformComp;

        engine::renderer::components::light_handle_t lightComponent;
    };
}

