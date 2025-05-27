//
// Created by Cem on 26.05.2022.
//
#pragma once
#include <engine/physics/components/CPhysicsComponent.h>
#include <engine/world/AActor.h>
#include <engine/callbacks/Event.h>
#include <engine/physics/vehicle/VehicleUtil.h>

namespace engine::physics
{
    class PhysicsSystem;
}

namespace engine::physics::components
{
    const std::pair<float, float> INPUT_RANGE = {-1.f, 1.f};
    const std::pair<float, float> OUTPUT_RANGE = {0.f, 1.f};

    static inline float mapInputToRange(float value, std::pair<float, float> inputRange, std::pair<float, float> outputRange)
    {
        return outputRange.first +
               (
                       (outputRange.second - outputRange.first) / (inputRange.second - inputRange.first) *
                       (value - inputRange.first)
               );
    }

    /// \brief Abstract class designed to be overridden for any kind of vehicle.
    struct CVehicle : public physics::components::CPhysicsComponent
    {
        /* Events */
        using event_t = typename engine::callbacks::Event<CVehicle, physx::PxVehicleDrive**>;

    public:
        friend PhysicsSystem;

        /// \brief Struct used in init(..) to initialize the component.
        struct ConstructionInfo : public engine::components::ConstructionInfo
        {
        public:
            vehicle::VehicleDesc vehicleDesc;
        };

        /// \brief Intended for internal usage only!
        /// \param id The id of the newly constructed component.
        /// \param owner The id of the entity that owns this component.
        explicit CVehicle(engine::components::id_t id = engine::components::invalid_id,
                          entities::id_t owner = entities::invalid_id);


        /// \brief Initializes the component using physics::components::CVehicle::ConstructionInfo.
        /// \param constructionInfo Info used to configure the component.
        void init(const engine::components::ConstructionInfo* constructionInfo) override;

        /// \brief Called on initialVisit(..) to create vehicle physics.
        /// \param constructionInfo Info used to configure the component.
        virtual void createVehiclePhysics() = 0;

        /// \brief Clean up.
        void destroyPhysics() override;

        /// \brief Updates the transform based on the physics world transform.
        void updateTransform() override;

        virtual void processInput(int key, int action) = 0;

        virtual void processInput(const GLFWgamepadstate& controllerState) = 0;

        void configureUserData();

        /// \brief Create vehicle here with the received transform date.
        ///
        /// Call this by subscribing to the correct event in CTransform.
        /// \param treeTraverser Traverser traversing the scene graph.
        void initialVisit(scene::TreeTraverser* traverser) override;

        /// \brief Update transform data.
        ///
        /// Call this by subscribing to the correct event in CTransform.
        /// \param treeTraverser Traverser traversing the scene graph.
        void visit(scene::TreeTraverser* traverser) override;

    protected:
        /// \brief Create actual PhysX drive here.
        virtual physx::PxVehicleDrive* createVehicle() = 0;

        /// \brief Creates basic friction data.
        /// \return Tire to surface friction pairs.
        virtual physx::PxVehicleDrivableSurfaceToTireFrictionPairs* createFrictionPairs();

        /// \brief  Creates the wheel mesh.
        /// \param width Width of the wheel.
        /// \param radius Radius of the wheel.
        /// \return The convex mesh that is going to be used in the simulation.
        virtual physx::PxConvexMesh* createWheelMesh(float width, float radius);

        /// \brief Creates the actual chassis mesh.
        /// \param vec3 The scale of the mesh.
        /// \return The convex mesh that is going to be used in the simulation.
        virtual physx::PxConvexMesh* createChassisMesh(const physx::PxVec3& vec3);

        /// \brief Create the rigid body that represents the vehicle.
        /// \param chassisData Chassis data.
        /// \param wheelMaterials Physics materials for the wheels.
        /// \param wheelMeshes Wheel meshes.
        /// \param numWheels Number of wheel.
        /// \param wheelSimFilterData Simulation filter data for the wheels.
        /// \param chassisMaterial Physics material for the wheel mesh.
        /// \param chassisMeshes Convex Mesh for the chassis.
        /// \param numChassisMeshes Number of meshes for the chassis.
        /// \param chassisSimFilterData Simulation filter data for the wheels.
        /// \return The rigidy body that represents the vehicle.
        virtual physx::PxRigidDynamic*
        createVehicleActor(const physx::PxVehicleChassisData& chassisData, physx::PxMaterial* wheelMaterials[20],
                           physx::PxConvexMesh* wheelMeshes[20], unsigned int numWheels,
                           const physx::PxFilterData& wheelSimFilterData, const physx::PxMaterial* chassisMaterial[1],
                           physx::PxConvexMesh* chassisMeshes[1], int numChassisMeshes, const physx::PxFilterData& chassisSimFilterData);

        /// \brief Setup the wheels.
        /// \param wheelMass Mass of the wheels.
        /// \param wheelMOI Moment of inertia.
        /// \param wheelRadius Radius of the wheels.
        /// \param wheelWidth Width of the wheels.
        /// \param numWheels Number of wheels.
        /// \param wheelCenterActorOffsets Offset to the rigid body.
        /// \param chassisCMOffset Center of mass offset.
        /// \param chassisMass Chassis mass.
        /// \param wheelsSimData Simulation filter data for wheels.
        virtual void setupWheels(float wheelMass, float wheelMOI, float wheelRadius, float wheelWidth,
                                 unsigned int numWheels, physx::PxVec3 wheelCenterActorOffsets[20],
                                 physx::PxVec3 chassisCMOffset, float chassisMass,
                                 physx::PxVehicleWheelsSimData* wheelsSimData) = 0;

        /// \brief Used to call the collision_event_t from the child class.
        /// \param event The event that should be fired.
        /// \param collisionInfo The payload of the event.
        void fireEvent(event_t& event, physx::PxVehicleDrive** vehicleDrive)
        {
            event.fire(vehicleDrive);
        }

    public:
        event_t event_physicsInitialized;

    protected:
        const engine::physics::PhysicsSystem* physicsSystem;

        physx::PxScene* pxScene;

        physx::PxPhysics* pxPhysics;

        engine::physics::vehicle::VehicleDesc vehicleDesc;

        physx::PxVehicleDrive* vehicle;

        physx::PxVehicleDrivableSurfaceToTireFrictionPairs* surfaceTirePairs;

        physx::PxMaterial* chassisMaterialDrivable;

        physx::PxMaterial* chassisMaterialNonDrivable;

        physx::PxTransform resetTransform;

        bool bIsVehicleInAir;

        bool bIsControlledByController;

        bool bIsInReverse;

        bool bIsMovingForwardSlowly;
    };
}