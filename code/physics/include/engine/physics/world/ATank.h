//
// Created by Cem on 15.06.2022.
//
#pragma once
#include <engine/world/AActor.h>
#include <engine/scene/components/CTransform.h>
#include "engine/physics/components/vehicles/CVehicleTank.h"

namespace engine::physics::world
{
    /// \brief Actor that has just a transform and a tank vehicle component
    class ATank : public engine::world::AActor
    {
    public:
        ATank(glm::mat4 transform, engine::world::World* world);

        void tick(float deltaTime) override;

        [[nodiscard]] const components::vehicleTank_handle_t& getTankComp() const;

        /// Just calls debugVehicle(..) of the four wheeled vehicle component
        /// \param debugInfo The debug info to pass on to the vehicle
        void debugVehicle(vehicle::DebugVehicleTank& debugInfo);

        /// Just calls updateDebugInfo(..) of the four wheeled vehicle component
        /// \param debugInfo The debug info to pass on to the vehicle
        void updateDebugInfo(vehicle::DebugVehicleTank& dbgInfo);

    protected:
        static physics::vehicle::VehicleDesc initVehicleDesc();

        engine::scene::components::transform_handle_t transformComp;

        engine::physics::components::vehicleTank_handle_t tankComp;

        engine::world::World::key_input_event_t::handle_t handle_keyInput;

        engine::world::World::controller_input_event_t::handle_t handle_controllerInput;
    };
}
