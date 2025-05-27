//
// Created by Cem on 29.05.2022.
//
#pragma once
#include "engine/physics/components/vehicles/CVehicle.h"

namespace engine::physics::components
{
    /// \brief Basic tank implementation.
    struct CVehicleTank : public physics::components::CVehicle
    {
    public:
        friend PhysicsSystem;

        /// \brief Intended for internal usage only!
        /// \param id The id of the newly constructed component.
        /// \param owner The id of the entity that owns this component.
        explicit CVehicleTank(engine::components::id_t id = engine::components::invalid_id,
                              entities::id_t owner = entities::invalid_id);

        void createVehiclePhysics() override;

        void tick(float deltaTime) override;

        void processInput(int key, int action) override;

        void processInput(const GLFWgamepadstate& controllerState) override;

        /// \brief Update debug info.
        /// \param dbgInfo Info to update.
        void updateDebugInfo(vehicle::DebugVehicleTank& dbgInfo);

        /// \brief Update vehicle based on debug info.
        /// \param dbgInfo Info to use the update.
        void debugVehicle(const vehicle::DebugVehicleTank& debugInfo);

        /// \brief Get vehicle debug data.
        /// \return Debug Info
        vehicle::DebugVehicleTank getDebugInfo();

    protected:
        physx::PxVehicleDriveTank* createVehicle() override;

        void setupWheels(float wheelMass, float wheelMOI, float wheelRadius, float wheelWidth,
                         unsigned int numWheels, physx::PxVec3 wheelCenterActorOffsets[20],
                         physx::PxVec3 chassisCMOffset, float chassisMass,
                         physx::PxVehicleWheelsSimData* wheelsSimData) override;

    private:
        static void computeWheelCenterActorOffsets(const physx::PxF32 wheelFrontZ, const physx::PxF32 wheelRearZ, const physx::PxVec3& chassisDims, const physx::PxF32 wheelWidth, const physx::PxF32 wheelRadius, const physx::PxU32 numWheels, physx::PxVec3* wheelCentreOffsets)
        {
            //chassisDims.z is the distance from the rear of the chassis to the front of the chassis.
            //The front has z = 0.5*chassisDims.z and the rear has z = -0.5*chassisDims.z.
            //Compute a position for the front wheel and the rear wheel along the z-axis.
            //Compute the separation between each wheel along the z-axis.
            const physx::PxF32 numLeftWheels = numWheels/2.0f;
            const physx::PxF32 deltaZ = (wheelFrontZ - wheelRearZ)/(numLeftWheels-1.0f);
            //Set the outside of the left and right wheels to be flush with the chassis.
            //Set the top of the wheel to be just touching the underside of the chassis.
            for(physx::PxU32 i = 0; i < numWheels; i+=2)
            {
                //Left wheel offset from origin.
                wheelCentreOffsets[i + 0] = physx::PxVec3((-chassisDims.x + wheelWidth)*0.5f, -(chassisDims.y/2 + wheelRadius), wheelRearZ + i*deltaZ*0.5f);
                //Right wheel offsets from origin.
                wheelCentreOffsets[i + 1] = physx::PxVec3((+chassisDims.x - wheelWidth)*0.5f, -(chassisDims.y/2 + wheelRadius), wheelRearZ + i*deltaZ*0.5f);
            }
        }

        void startBrakeMode();

        void processAutoReverse(bool& bToggleAutoReverse, bool& bNewIsMovingForwardSlowly);

    protected:
        physx::PxVehicleDriveTank* vehicleTank;

        physx::PxVehicleDriveTankRawInputData vehicleInputData;

        physx::PxCooking* pxCooking;

        bool bStartedRightTurn, bStartedLeftTurn, bBreakedLeft, bBreakedRight;
    };

    using vehicleTankComponent_container_t = typename engine::components::component_pool_t<CVehicleTank>;
    using vehicleTank_handle_t = typename vehicleTankComponent_container_t::handle_t;
    using vehicleTankContainer_event_handle = typename vehicleTankComponent_container_t::event_t::handle_t;
}