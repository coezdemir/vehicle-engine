//
// Created by CemOe on 10.10.2021.
//
#pragma once
#include <engine/Engine.h>
#include <engine/physics/components/collision/CSphereCollision.h>
#include <engine/physics/components/collision/CBoxCollision.h>
#include <engine/physics/components/collision/CComplexCollision.h>
#include <engine/physics/components/vehicles/CVehicle4Wheeled.h>
#include <engine/physics/components/vehicles/CVehicleTank.h>
#include <engine/physics/components/vehicles/CRaycastVehicle.h>
#include <engine/physics/vehicle/VehicleManager.h>
#include <engine/physics/collision/CollisionCallback.h>
#include <engine/physics/components/collision/CHeightField.h>

namespace engine::physics
{
    /// \brief Subsystem managing PhysX and its physics bodies
    class PhysicsSystem : public system::System
    {
    public:
        /* Events */
        using event_void_t = typename callbacks::Event<PhysicsSystem>;
        using event_float_t = typename callbacks::Event<PhysicsSystem, float>;

        explicit PhysicsSystem(engine::Engine* engine, physx::PxSceneDesc sceneDesc);

        /// \brief Registers the system to the engine.
        /// \param engine Pointer to the engine.
        void registerSystem(Engine* engine) override;

        /// \brief Helper function to create a convex mesh for the physics simulation.
        /// \param vertices Vertices of the mesh. Max 32.
        /// \param numVertices Number of vertices of the convex mesh.
        /// \return The pointer to the newly created convex mesh.
        physx::PxConvexMesh* createConvexMesh(physx::PxVec3 vertices[32], int numVertices) const;

        /// Returns the default physics material.
        /// \return Default physics material.
        [[nodiscard]] physx::PxMaterial* getDefaultMaterial() const;

        /// Standard material for a drivable vehicle chassis.
        /// \return Physics Material
        [[nodiscard]] physx::PxMaterial* getChassisMaterialDrivable() const;

        /// Standard material for a non drivable vehicle chassis.
        /// \return Physics Material
        [[nodiscard]] physx::PxMaterial* getChassisMaterialNonDrivable() const;

        /// Returns the default physics material.
        /// \return Default physics material.
        [[nodiscard]] physx::PxMaterial* getMaterial() const;

        /// Returns PhysX Allocator
        /// \return Allocator
        [[nodiscard]] physx::PxDefaultAllocator* getPxAllocator();

        /// Returns PhysX scene
        /// \return PxScene
        [[nodiscard]] physx::PxScene* getPxScene() const;

        /// Returns PhysX physics
        /// \return PxPhysics
        [[nodiscard]] physx::PxPhysics* getPxPhysics() const;

    private:
        void startup(Engine* engine);

        void tick(float deltaTime);

        void shutdown(Engine* engine);

        /// \brief Steps PhysX vehicle simulation
        void stepVehiclePhysics();

    public:
        event_void_t event_afterPhysicsTick;

        event_void_t event_physicsShutdown;

        event_float_t event_stepPhysics;

    private:
        float accumulator;

        const float STEP_SIZE = 1.f / 60.f;

        engine::Engine::event_engineptr_t::handle_t handle_startup;

        engine::Engine::event_void_t::handle_t handle_beginPlay;

        engine::Engine::tick_event_t::handle_t handle_tick;

        engine::Engine::event_engineptr_t::handle_t handle_shutdown;

        /* Necessary members for PhysX */

        physx::PxDefaultAllocator pxAllocator;

        physx::PxDefaultErrorCallback pxErrorCallback;

        physx::PxFoundation* pxFoundation;

        physx::PxCooking* pxCooking;

        physx::PxPhysics* pxPhysics;

        physx::PxDefaultCpuDispatcher* pxDispatcher;

        physx::PxScene* pxScene;

        physx::PxMaterial* defaultMaterial;

        engine::physics::collision::CollisionCallback collisionCallback;

        physx::PxMaterial* chassisMaterialDrivable;

        physx::PxMaterial* chassisMaterialNonDrivable;

        std::vector<physx::PxMaterial*> physicsMaterials;

        /* Default values for PhysX properties to use */

        float defaultStaticFriction;

        float defaultDynamicFriction;

        float defaultRestitution;

        float defaultDensity;

        float defaultAngularDamping;

        /* PhysX Visual Debugger */

        physx::PxPvd* pvd = nullptr;

        const std::string PVD_HOST = "127.0.0.1";

        const int PVD_PORT = 5425;

        const unsigned int PVD_TIMEOUT = 10;

        physics::components::raycastVehicle_container_t* vehicleContainer;

        physics::components::raycastWheeledVehicle_container_t* vehicleWheeledContainer;

        physics::components::vehicleTankComponent_container_t* tankContainer;

        physics::components::boxCollision_container_t* boxCollisionContainer;

        physics::components::sphereCollision_container_t* sphereCollisionContainer;

        physics::components::complexCollision_container_t* complexCollisionContainer;

        physics::components::heightField_container_t* heightFieldContainer;

        engine::physics::vehicle::VehicleManager vehicleManager;

        std::map<physics::components::raycastVehicle_container_t::handle_t, event_void_t::handle_t> raycastVehicleHandles_physicsEvent;

        std::map<physics::components::raycastWheeledVehicle_container_t::handle_t, event_void_t::handle_t> raycastWheeledHandles_physicsEvent;

        std::map<physics::components::vehicleTankComponent_container_t::handle_t, event_void_t::handle_t> tankHandles_physicsEvent;

        std::map<physics::components::boxCollision_container_t::handle_t, event_void_t::handle_t> boxCollisionHandles_physicsEvent;

        std::map<physics::components::sphereCollision_container_t::handle_t, event_void_t::handle_t> sphereCollisionHandles_physicsEvent;

        std::map<physics::components::complexCollision_container_t::handle_t, event_void_t::handle_t> complexCollisionHandles_physicsEvent;

        std::map<physics::components::raycastVehicle_container_t::handle_t, event_float_t::handle_t> raycastVehicleHandles_stepEvent;

        std::map<physics::components::raycastWheeledVehicle_container_t::handle_t, event_float_t::handle_t> raycastWheeledHandles_stepEvent;

        std::map<physics::components::vehicleTankComponent_container_t::handle_t, event_float_t::handle_t> tankHandles_stepEvent;

        std::map<physics::components::boxCollision_container_t::handle_t, event_float_t::handle_t> boxCollisionHandles_stepEvent;

        std::map<physics::components::sphereCollision_container_t::handle_t, event_float_t::handle_t> sphereCollisionHandles_stepEvent;

        std::map<physics::components::complexCollision_container_t::handle_t, event_float_t::handle_t> complexCollisionHandles_stepEvent;

        std::map<physics::components::heightField_container_t::handle_t , event_void_t::handle_t> heightFieldHandles_physicsEvent;
    };
}