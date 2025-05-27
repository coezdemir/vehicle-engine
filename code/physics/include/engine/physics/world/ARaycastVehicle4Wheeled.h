//
// Created by CemOe on 30.09.2021.
//
#pragma once
#include <engine/world/AActor.h>
#include "engine/physics/components/vehicles/CVehicle4Wheeled.h"
#include <engine/scene/components/CTransform.h>
#include "engine/physics/components/collision/CSphereCollision.h"
#include <engine/renderer/components/CModel.h>
#include "engine/physics/components/collision/CComplexCollision.h"
#include <engine/renderer/Mesh.h>

namespace engine::physics::world
{
    /// \brief Actor that has just a transform and a four wheeled vehicle
    class ARaycastVehicle4Wheeled : public engine::world::AActor
    {
    public:
        ARaycastVehicle4Wheeled(glm::mat4 transform, engine::world::World* world);

        void tick(float deltaTime) override;

        /// Just calls debugVehicle(..) of the four wheeled vehicle component
        /// \param debugInfo The debug info to pass on to the vehicle
        void debugVehicle(vehicle::DebugRaycastVehicle4W& debugInfo);

        /// Just calls updateDebugInfo(..) of the four wheeled vehicle component
        /// \param debugInfo The debug info to pass on to the vehicle
        void updateDebugInfo(vehicle::DebugRaycastVehicle4W& dbgInfo);

        [[nodiscard]] glm::vec4 getLocation();

        [[nodiscard]] const components::raycastWheeledVehicle_handle_t& getRaycastVehicleComp() const;

    protected:
        static physics::vehicle::VehicleDesc initVehicleDesc();

        engine::scene::components::transform_handle_t transformComp;

        engine::physics::components::raycastWheeledVehicle_handle_t raycastVehicleComp;

        engine::world::World::key_input_event_t::handle_t handle_keyInput;

        engine::world::World::controller_input_event_t::handle_t handle_controllerInput;
    };
}
