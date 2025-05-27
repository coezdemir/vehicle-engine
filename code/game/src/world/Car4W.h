//
// Created by Cem on 24.05.2022.
//
#pragma once

#include "../gamemode/PointGamemode.h"
#include "engine/physics/Physics.h"
#include <engine/physics/world/ARaycastVehicle4Wheeled.h>
#include <engine/renderer/components/CCamera.h>

namespace game::actors
{
    class Car4W : public engine::physics::world::ARaycastVehicle4Wheeled
    {
        friend game::Game;
        friend game::gamemode::PointGamemode;
    public:
        Car4W(glm::mat4 transform, engine::world::World* world);

    private:
        engine::scene::components::transform_handle_t camTransformComp;

        engine::renderer::components::cameraComponent_handle_t cameraComp;
    };
}
