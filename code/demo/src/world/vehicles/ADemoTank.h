//
// Created by Cem on 26.07.2022.
//
#pragma once
#include <engine/physics/world/ATank.h>
#include <engine/renderer/components/CModel.h>

namespace demo::world
{
    class ADemoTank : public engine::physics::world::ATank
    {
    public:
        ADemoTank(glm::mat4 transform, engine::world::World* world);

        void activateControls();

        void deactivateControls();

    private:
        engine::renderer::components::model_handle_t meshComp;

        engine::scene::components::transform_handle_t meshTransformComp;

        bool bControlsActive;
    };
}