//
// Created by Cem on 13.04.2022.
//
#pragma once
#include "engine/physics/components/CPhysicsComponent.h"
#include "engine/components/ComponentPool.h"
#include "engine/physics/vehicle/VehicleUtil.h"
#include "GLFW/glfw3.h"

#define FORWARD_STEER_PAIRS 1

namespace engine::physics::components
{
    /// \brief Basic raycast vehicle for an arcade style game
    struct CRaycastVehicle : public physics::components::CPhysicsComponent
    {
        /* Events */
        using event_void_t = typename engine::callbacks::ComponentEvent<CRaycastVehicle>;

    public:
        friend PhysicsSystem;

        /// \brief Struct used in init(..) to initialize the component.
        struct ConstructionInfo : public engine::components::ConstructionInfo
        {
        public:
            glm::vec3 boxHalfExtends;
            float chassisMass;
            float suspensionLength;
            float suspensionStrength;
            float suspensionDampening;
            glm::vec3 centerOfMassOffset;
            float accelerationFactor;
            float friction;
            float forwardVsSteer[FORWARD_STEER_PAIRS * 2];
        };

        /// \brief Intended for internal usage only!
        /// \param id The id of the newly constructed component.
        /// \param owner The id of the entity that owns this component.
        explicit CRaycastVehicle(engine::components::id_t id = engine::components::invalid_id,
                                 entities::id_t owner = entities::invalid_id);

        /// \brief Initializes the component using physics::components::CRaycastVehicle::ConstructionInfo.
        /// \param constructionInfo Info used to configure the component.
        void init(const engine::components::ConstructionInfo* constructionInfo) override;

        void tick(float deltaTime) override;

        void activate() override;

        void deactivate() override;

        /// \brief Reset transform to the specified reset transform
        void reset();

        /// \brief Applay boost like force.
        /// \param accelerationFactor Factor to scale the force with.
        void boost(float accelerationFactor);

        void processKeyInput(int key, int action);

        void processControllerInput(GLFWgamepadstate controllerState);

        /// \brief Update vehicle based on debug info.
        /// \param dbgInfo Info to use the update.
        void updateUsingDbgInfo(vehicle::DebugRaycastVehicle& dbgInfo);

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

        void setResetTransform(const physx::PxTransform& resetTransform);

        void setUseControllerInput(bool bUseControllerInput);

        [[nodiscard]] bool isInAir() const;

        [[nodiscard]] float getForwardSpeed() const;

        [[nodiscard]] const glm::vec3& getBoxHalfExtends() const;

        [[nodiscard]] vehicle::DebugRaycastVehicle getDebugInfo();

        void printLoc();

        void handleForwardInput(int action);

        void handleBackwardInput(int action);

        void handleSteerLeftInput(int action);

        void handleSteerRightInput(int action);


    protected:
        void initPhysics();

        void createVehicleChassis();

        void stepPhysics(float stepSize) override;

        void applyTraction(float deltaTime);

        void evaluateInput(float deltaTime);

        float matchForwardToSteer();

    public:
        event_void_t event_physicsInitialized;

    protected:
        physx::PxRigidDynamic* rigidDynamic;

        physx::PxTransform localTransform;

        glm::vec3 boxHalfExtends;

        glm::vec3 centerOfMassOffset;

        float chassisMass;

        physx::PxRaycastBuffer hit;

        float suspensionLength;

        float suspensionStrength;

        float suspensionDampening;

        float accelerationFactor;

        float turningRate;

        float forwardVsSteer[FORWARD_STEER_PAIRS * 2];

        float friction;

        bool bIsInAir;

        physx::PxTransform resetTransform;

        bool bUseControllerInput;

        engine::physics::vehicle::VehicleKeyInput keyInput;

        engine::physics::vehicle::VehicleControllerInput controllerInput;
    };

    using raycastVehicle_container_t = typename engine::components::component_pool_t<CRaycastVehicle>;
    using raycastVehicle_handle_t = typename raycastVehicle_container_t::handle_t;
    using raycastVehicle_event_handle = typename raycastVehicle_container_t::event_t::handle_t;
}