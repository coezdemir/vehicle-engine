//
// Created by Cem on 26.07.2022.
//
#pragma once
#include <engine/physics/Physics.h>
#include <engine/physics/world/ARaycastVehicle4Wheeled.h>

namespace demo::world
{
    class ADemo4WVehicle : public engine::physics::world::ARaycastVehicle4Wheeled
    {
    public:
        ADemo4WVehicle(glm::mat4 transform, engine::world::World* world);

        void activateControls();

        void deactivateControls();

    private:
        engine::renderer::components::model_handle_t meshComp;

        bool bControlsActive;
    };
}