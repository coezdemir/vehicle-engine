//
// Created by Cem on 06.04.2022.
//
#pragma once
#include "VehicleUtil.h"

#define MAX_NUM_WHEELS 20
#define MAX_NUM_VEHICLES_PER_TYPE 4
#define VEHICLE_TYPES 2
#define FOUR_WHEELED_DATA_INDEX 0
#define TANK_DATA_INDEX 1

namespace engine::physics
{
    class PhysicsSystem;
}

namespace engine::physics::vehicle
{
    /// \brief Manages and updates the vehicles.
    class VehicleManager
    {
    public:
        VehicleManager() = default;

        VehicleManager(engine::physics::PhysicsSystem* physicsSystem);

        virtual ~VehicleManager();

        /// \brief Create the needed data for the simulation
        void initVehicleSimulation(int dataIndex);

        /// \brief Register your vehicle here.
        /// \param vehicle Vehicle to register.
        virtual void registerVehicle(physx::PxVehicleDrive** vehicle);

        /// \brief Step vehicle simulation.
        /// \param timestep
        void stepVehiclePhysics(float timestep);

        [[nodiscard]] virtual int getNbOfRegisteredVehicles(int vehicleType) const;

        [[nodiscard]] bool isInitialized() const;

    protected:
        virtual void createSceneQueryData();

        virtual physx::PxVehicleDrivableSurfaceToTireFrictionPairs* createFrictionPairs();

        engine::physics::PhysicsSystem* physicsSystem;

        physx::PxDefaultAllocator* pxAllocator;

        physx::PxScene* pxScene;

        int numberOfVehicles[VEHICLE_TYPES];

        physx::PxVehicleDrive** fourwheeledVehicle[MAX_NUM_VEHICLES_PER_TYPE] = {nullptr, nullptr, nullptr, nullptr};

        physx::PxVehicleDrive** tankVehicles[MAX_NUM_VEHICLES_PER_TYPE] = {nullptr, nullptr, nullptr, nullptr};

        engine::physics::vehicle::VehicleSceneQueryData* vehicleSceneQueryData[VEHICLE_TYPES];

        physx::PxBatchQuery* batchQuery[VEHICLE_TYPES];

        physx::PxVehicleWheelQueryResult vehicleWheelQueryResults[MAX_NUM_VEHICLES_PER_TYPE];

        physx::PxVehicleDrivableSurfaceToTireFrictionPairs* frictionPairs[VEHICLE_TYPES] = {nullptr, nullptr};

        static constexpr int BATCH_ID[VEHICLE_TYPES] = {FOUR_WHEELED_DATA_INDEX, TANK_DATA_INDEX};

        bool bIsInitialized;

        bool bAutomaticInit;
    };
}
