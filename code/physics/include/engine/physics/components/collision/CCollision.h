//
// Created by CemOe on 27.08.2021.
//
#pragma once
#include <engine/scene/components/CTransform.h>
#include <engine/scene/TreeTraverser.h>
#include <engine/world/AActor.h>
#include <engine/callbacks/ComponentEvent.h>
#include <engine/physics/components/CPhysicsComponent.h>

namespace engine::physics::collision
{
    class CollisionCallback;
}

namespace engine::physics::components
{
    /// \brief Provides information about a collision.
    struct CollisionInfo
    {
        engine::world::AActor* otherActor = nullptr;
        engine::physics::components::CPhysicsComponent* otherComponent = nullptr;
        physx::PxRigidActor* otherBody = nullptr;
        physx::PxShape* otherShape = nullptr;
    };

    /// \brief Abstract parent class designed to be overridden for any kind of collision interaction.
    ///
    /// Provides various functions to easily create and manage a collision body.
    /// For any kind of question regarding the physics engine please refer to the PhysX documentation :
    /// https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Index.html
    struct CCollision : public physics::components::CPhysicsComponent
    {
        friend engine::physics::collision::CollisionCallback;
        friend PhysicsSystem;

    public:
        /* Events */
        using collision_event_t = typename engine::callbacks::ComponentEvent<struct CCollision, const CollisionInfo&>;

        /// \brief Struct used in init(..) to initialize the component.
        struct ConstructionInfo : public engine::components::ConstructionInfo
        {
        public:
            bool bIsDynamic;
            bool bIsKinematic;
            bool bUpdateLocation;
            bool bIsSimulationShape;
            bool bShouldDeactivateAfterCollision;
            float timeActive;
            float mass = 0;
            bool bActiveFromStart;
            physx::PxFilterData simulationFilterData;
            physx::PxFilterData qryFilterData;
        };

        /// \brief Intended for internal usage only!
        /// \param id The id of the newly constructed component.
        /// \param owner The id of the entity that owns this component.
        explicit CCollision(const engine::components::id_t id = engine::components::invalid_id,
                            const entities::id_t owner = entities::invalid_id)
                : CPhysicsComponent(id, owner)
        {

        }

        ~CCollision()
        {
            event_onOverlap.unsubscribe();
            event_onHit.unsubscribe();
        }

        CCollision& operator=(CCollision&& other) noexcept
        {
            if (this != &other)
            {
                CPhysicsComponent::operator=(std::move(other));
                event_onOverlap = other.event_onOverlap;
                event_onHit = other.event_onHit;
                bIsDynamic = other.bIsDynamic;
                bIsKinematic = other.bIsKinematic;
                bIsSimulationShape = other.bIsSimulationShape;
                bShouldDeactivateAfterCollision = other.bShouldDeactivateAfterCollision;
                restitution = other.restitution;
                friction = other.friction;
                mass = other.mass;
                timeActive = other.timeActive;
                bShouldDeactivateAfterTime = other.bShouldDeactivateAfterTime;
                bUpdateLocation = other.bUpdateLocation;
                bActiveFromStart = other.bActiveFromStart;
                simulationFilterData = other.simulationFilterData;
                qryFilterData = other.qryFilterData;

                other.event_onOverlap.unsubscribe();
                other.event_onHit.unsubscribe();
                return *this;
            }
            return *this;
        }

        /// \brief Initializes the component using physics::components::CPhysicsComponent::ConstructionInfo.
        /// \param constructionInfo Info used to configure the component.
        void init(const engine::components::ConstructionInfo* const constructionInfo) override
        {
            CComponent::init(constructionInfo);

            auto* const collisionConstructionInfo =
                    dynamic_cast<const engine::physics::components::CCollision::ConstructionInfo*>(constructionInfo);
            assert(collisionConstructionInfo
                   && "Please use the correct ConstructionInfo: BoxCollisionComponent::ConstructionInfo");

            bIsDynamic = collisionConstructionInfo->bIsDynamic;
            bIsKinematic = collisionConstructionInfo->bIsKinematic;
            bIsSimulationShape = collisionConstructionInfo->bIsSimulationShape;
            bShouldDeactivateAfterCollision = collisionConstructionInfo->bShouldDeactivateAfterCollision;
            timeActive = collisionConstructionInfo->timeActive;
            mass = collisionConstructionInfo->mass;
            bUpdateLocation = collisionConstructionInfo->bUpdateLocation;
            bActiveFromStart = collisionConstructionInfo->bActiveFromStart;
            simulationFilterData = collisionConstructionInfo->simulationFilterData;
            qryFilterData = collisionConstructionInfo->qryFilterData;

            bShouldDeactivateAfterTime = timeActive > 0.f;
        }

        /// \brief Create physics body with the received transform date.
        ///
        /// Call this by subscribing to the correct event in CTransform.
        /// \param treeTraverser Traverser traversing the scene graph.
        void initialVisit(engine::scene::TreeTraverser* treeTraverser) override
        {
            bInitialTraversed = true;

            initPhysics(treeTraverser->getStackTop());

            if (bActiveFromStart)
                activate();
        }

        /// \brief Used to update various transforms based on configuration.
        ///
        /// Updates either transform of CTransform it is attached to.
        /// The next location the kinematic body should move to.
        /// Or to force a new location on to the body.
        /// \param treeTraverser Traverser traversing the scene graph.
        void visit(scene::TreeTraverser* treeTraverser) override
        {
            if (!bInitialTraversed)
                initialVisit(treeTraverser);
            
            if (bIsActive)
            {
                if (bIsDynamic)
                {
                    if (!bIsKinematic)
                    {
                        if (!bNeedsUpdate)
                            return;

                        const engine::scene::components::transform_handle_t transformComponent =
                                treeTraverser->getCurrTransformComp();

                        treeTraverser->popStack();

                        transformComponent->setTransform(transform);

                        const glm::mat4 newLocal = glm::inverse(treeTraverser->getStackTop()) * transform;
                        transformComponent->setLocalTransform(newLocal);

                        treeTraverser->pushStack(transformComponent->getLocalTransform());
                        bNeedsUpdate = false;
                    }
                    else
                    {
                        const scene::components::transform_handle_t transformComponent =
                                treeTraverser->getCurrTransformComp();

                        transform = transformComponent->getTransform();

                        if (rigidBody)
                        {
                            const physx::PxTransform newTransform = glmToPhysx(transform);

                            if (auto* rigidDynamic = rigidBody->is<physx::PxRigidDynamic>())
                                rigidDynamic->setKinematicTarget(newTransform);
                        }
                    }

                }
                else if (bUpdateLocation)
                {
                    const scene::components::transform_handle_t transformComponent =
                            treeTraverser->getCurrTransformComp();

                    transform = transformComponent->getTransform();

                    if (rigidBody)
                    {
                        const physx::PxTransform newTransform = glmToPhysx(transform);
                        rigidBody->setGlobalPose(newTransform);
                    }
                }
            }

        }

        /// \brief Updates the transform based on the physics world transform.
        void updateTransform() override
        {
            if (bIsDynamic && !bIsKinematic)
                CPhysicsComponent::updateTransform();
        }

        /// \brief Deactivates the physics body if it is configured to.
        ///
        /// Otherwise does nothing and exists to be overridden.
        /// \param deltaTime Time passed between frames.
        void tick(const float deltaTime) override
        {
            if (bShouldDeactivateAfterTime)
                if (timeActive - deltaTime < 0.f)
                    deactivate();
        }

    protected:
        /// \brief Used to call the collision_event_t from the child class.
        /// \param event The event that should be fired.
        /// \param collisionInfo The payload of the event.
        static void fireCollisionEvent(collision_event_t* const event, const CollisionInfo& collisionInfo)
        {
            event->fire(collisionInfo);
        }

    public:
        collision_event_t event_onOverlap;

        collision_event_t event_onHit;

    protected:
        bool bIsDynamic;

        bool bIsKinematic;

        bool bIsSimulationShape;

        bool bShouldDeactivateAfterCollision;

        float restitution;

        float friction;

        float mass;

        float timeActive;

        bool bShouldDeactivateAfterTime;

        bool bUpdateLocation;

        bool bActiveFromStart;

        physx::PxFilterData simulationFilterData;

        physx::PxFilterData qryFilterData;
    };
}