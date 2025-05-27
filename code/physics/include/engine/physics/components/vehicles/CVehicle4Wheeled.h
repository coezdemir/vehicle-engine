//
// Created by CemOe on 19.09.2021.
//
#pragma once

#include "CVehicle.h"
#include "engine/world/AActor.h"
#include "engine/physics/windows/WDebugWindow.h"
#include "engine/callbacks/Event.h"

namespace engine::physics::components
{
    /// \brief Basic 4 wheeled vehicle
    struct CVehicle4Wheeled : public physics::components::CVehicle
    {
    public:
        friend PhysicsSystem;

        /// \brief Intended for internal usage only!
        /// \param id The id of the newly constructed component.
        /// \param owner The id of the entity that owns this component.
        explicit CVehicle4Wheeled(engine::components::id_t id = engine::components::invalid_id,
                                  entities::id_t owner = entities::invalid_id);

        void createVehiclePhysics() override;

        void tick(float deltaTime) override;

        void processInput(int key, int action) override;

        void processInput(const GLFWgamepadstate& controllerState) override;

        /// \brief Update debug info.
        /// \param dbgInfo Info to update.
        void updateDebugInfo(vehicle::DebugRaycastVehicle4W& dbgInfo);

        /// \brief Update vehicle based on debug info.
        /// \param dbgInfo Info to use the update.
        void debugVehicle(vehicle::DebugRaycastVehicle4W& debugInfo);

        /// \brief Get vehicle debug data.
        /// \return Debug Info
        vehicle::DebugRaycastVehicle4W getDebugInfo();

    protected:
        physx::PxVehicleDrive4W* createVehicle() override;

        void computeWheelCenterActorOffsets4W(float wheelFrontZ, float wheelRearZ,
                                              physx::PxVec3 chassisDims,
                                              float wheelWidth, float wheelRadius,
                                              unsigned int numWheels,
                                              physx::PxVec3 pVec3[20]);

        void setupWheels(float wheelMass, float wheelMOI, float wheelRadius, float wheelWidth,
                                       unsigned int numWheels, physx::PxVec3 wheelCenterActorOffsets[20],
                                       physx::PxVec3 chassisCMOffset, float chassisMass,
                                       physx::PxVehicleWheelsSimData* wheelsSimData) override;

        void processAutoReverse(bool& toggleAutoReverse, bool& newIsMovingForwardSlowly);

        physx::PxVehicleDrive4W* vehicle4W;

        physx::PxVehicleDrive4WRawInputData vehicleInputData;
    };

    using raycastWheeledVehicle_container_t = typename engine::components::component_pool_t<CVehicle4Wheeled>;
    using raycastWheeledVehicle_handle_t = typename raycastWheeledVehicle_container_t::handle_t;
    using raycastWheeledVehicle_event_handle = typename raycastWheeledVehicle_container_t::event_t::handle_t;
}