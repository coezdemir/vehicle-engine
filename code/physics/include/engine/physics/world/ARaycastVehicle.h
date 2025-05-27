//
// Created by Cem on 13.04.2022.
//
#pragma once
#include <engine/world/AActor.h>
#include "engine/physics/components/vehicles/CRaycastVehicle.h"
#include <engine/scene/components/CTransform.h>
#include <engine/renderer/components/CModel.h>
#include <engine/renderer/Mesh.h>

namespace engine::physics::world
{
    /// \brief Actor that has just a transform and a raycast vehicle
    class ARaycastVehicle : public engine::world::AActor
    {
    public:
        ARaycastVehicle(glm::mat4 transform, engine::world::World* world);

        void tick(float deltaTime) override;

        [[nodiscard]] glm::vec4 getLocation();

        [[nodiscard]] const physics::components::raycastVehicle_handle_t& getRaycastVehicleComp() const;

    protected:
        engine::scene::components::transform_handle_t transformComp;

        engine::physics::components::raycastVehicle_handle_t raycastVehicleComp;

        engine::world::World::key_input_event_t::handle_t handle_keyInput;

        engine::world::World::controller_input_event_t::handle_t handle_controllerInput;
    };
}