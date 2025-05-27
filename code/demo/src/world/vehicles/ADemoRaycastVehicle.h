//
// Created by Cem on 26.07.2022.
//
#pragma once
#include <engine/physics/world/ARaycastVehicle.h>

namespace demo::world
{
    class ADemoRaycastVehicle : public engine::physics::world::ARaycastVehicle
    {
    public:
        ADemoRaycastVehicle(glm::mat4 transform, engine::world::World* world);

        void activateControls();

        void deactivateControls();

    private:
        engine::renderer::components::model_handle_t meshComp;

        bool bControlsActive;
    };
}
