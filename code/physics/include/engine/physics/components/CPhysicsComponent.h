//
// Created by Cem on 26.02.2022.
//
#pragma once

#include <engine/components/CComponent.h>
#include <engine/scene/ITraversableObject.h>
#include <PxConfig.h>
#include <PxPhysicsAPI.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <engine/callbacks/ComponentEvent.h>
#include <iostream>

static inline glm::vec3 physxToGLM(const physx::PxVec3& v)
{
    return {v.x, v.y, v.z};
}

static inline glm::quat physxToGLM(const physx::PxQuat& q)
{
    return {q.w, q.x, q.y, q.z};
}

static inline glm::mat4 physxToGLM(const physx::PxTransform& t)
{
    glm::mat4 glmTransform = glm::mat4(1.f);
    glm::vec3 translation = physxToGLM(t.p);
    glm::quat rotation = physxToGLM(t.q);
    glmTransform = glm::translate(glmTransform, translation);
    glmTransform = glmTransform * glm::toMat4(rotation);
    return glmTransform;
}

static inline physx::PxVec3 glmToPhysx(const glm::vec3& v)
{
    return {v.x, v.y, v.z};
}

static inline physx::PxTransform glmToPhysx(const glm::mat4& t)
{
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(t, scale, rotation, translation, skew, perspective);

    physx::PxVec3 pxTranslation = physx::PxVec3(translation.x, translation.y, translation.z);
    physx::PxQuat pxRotation = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);

    return {pxTranslation, pxRotation};
}

static inline physx::PxQuat glmToPhysx(const glm::quat& q)
{
    return {q.x, q.y, q.z, q.w};
}

static inline void
decomposeTransform(const glm::mat4 transform, glm::vec3& outTranslation, glm::quat& outRotation, glm::vec3& outScale)
{
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform, outScale, outRotation, outTranslation, skew, perspective);
}

static inline physx::PxVec3 projectVector(const physx::PxVec3& vectorToProject, const physx::PxVec3& planeNormal)
{
    return vectorToProject - (vectorToProject.dot(planeNormal) / (glm::pow(planeNormal.magnitude(), 2))) * planeNormal;
}

namespace engine::physics
{
    class PhysicsSystem;
}

namespace engine::physics::components
{
    /// \brief Abstract class designed to be overridden for any kind of physics interaction.
    ///
    /// Provides various functions to easily interact with the physics world.
    /// For any kind of question regarding the physics engine please refer to the PhysX documentation :
    /// https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Index.html
    struct CPhysicsComponent : public engine::components::CComponent, public engine::scene::ITraversableObject
    {
        /* Events */
        using event_rigidActor_t = typename engine::callbacks::ComponentEvent<CPhysicsComponent, const physx::PxRigidActor*>;
        using event_float_t = typename engine::callbacks::ComponentEvent<CPhysicsComponent, const float>;
        using event_void_t = typename engine::callbacks::ComponentEvent<CPhysicsComponent>;

    public:
        /// \brief Intended for internal usage only!
        ///
        /// \param id The id of the newly constructed component.
        /// \param owner The id of the entity that owns this component.
        explicit CPhysicsComponent(engine::components::id_t id = engine::components::invalid_id,
                                   entities::id_t owner = entities::invalid_id)
                : CComponent(id, owner),
                  transform(glm::mat4(1.f)), bIsActive(false), bPhysicsInitialized(false),
                  rigidBody(nullptr), shape(nullptr), bNeedsUpdate(false), pxDefaultMat(nullptr)
        {

        }

        ~CPhysicsComponent() override
        {
            if (bIsActive)
                destroyPhysics();

            event_onRigidBodyCreated.unsubscribe();
            event_onPhysicsStep.unsubscribe();
            event_onPhysicsStepEnd.unsubscribe();
        }

        CPhysicsComponent& operator=(CPhysicsComponent&& other) noexcept
        {
            if (this != &other)
            {
                // Component
                owner = other.owner;
                id = other.id;
                deleted = other.deleted;
                instantiator = other.instantiator;

                other.owner = entities::invalid_id;
                other.id = engine::components::invalid_id;
                other.instantiator = nullptr;

                // ITraversableObject
                bInitialTraversed = other.bInitialTraversed;

                other.bInitialTraversed = false;

                // Physics Component
                pxPhysics = other.pxPhysics;
                pxScene = other.pxScene;
                pxDefaultMat = other.pxDefaultMat;
                transform = other.transform;
                rigidBody = other.rigidBody;
                shape = other.shape;
                bNeedsUpdate = other.bNeedsUpdate;
                bIsActive = other.bIsActive;
                bPhysicsInitialized = other.bPhysicsInitialized;
                event_onRigidBodyCreated = other.event_onRigidBodyCreated;
                event_onPhysicsStep = other.event_onPhysicsStep;
                event_onPhysicsStepEnd = other.event_onPhysicsStepEnd;
                if (rigidBody)
                { rigidBody->userData = this; }

                other.rigidBody = nullptr;
                other.shape = nullptr;
                other.bNeedsUpdate = false;
                other.bIsActive = false;
                other.bPhysicsInitialized = false;
                other.event_onRigidBodyCreated.unsubscribe();
                other.event_onPhysicsStep.unsubscribe();
                other.event_onPhysicsStepEnd.unsubscribe();
                return *this;
            }
            return *this;
        }

        /// \brief Updates the transform based on the physics world transform.
        virtual void updateTransform()
        {
            if (rigidBody && rigidBody->is<physx::PxRigidDynamic>() && bIsActive)
            {
                physx::PxTransform newTransform = rigidBody->getGlobalPose();
                auto mat44 = physx::PxMat44(newTransform);

                for (int i = 0; i < 4; ++i)
                {
                    for (int j = 0; j < 4; ++j)
                    {
                        transform[i][j] = mat44[i][j];
                    }
                }

                bNeedsUpdate = true;
            }
        }

        /// \brief Places the body in the physics world.
        virtual void activate()
        {
            if (bPhysicsInitialized && !bIsActive)
            {
                addToWorld();
                bIsActive = true;
            }
        }

        /// \brief Removes the body in the physics world.
        virtual void deactivate()
        {
            if (bIsActive)
            {
                removeFromWorld();
                bIsActive = false;
            }
        }

        /// \brief Adds the specified force to the body.
        /// \param force Vector of force that is going to applied.
        /// \param forceMode Determines how the force is going to be applied.
        virtual void addForce(const physx::PxVec3& force, const physx::PxForceMode::Enum forceMode)
        {
            if (auto* rigidDynamic = rigidBody->is<physx::PxRigidDynamic>())
                rigidDynamic->addForce(force, forceMode);
        }

        /// \brief Adds the specified torque to the body.
        /// \param force Vector of torque that is going to applied.
        /// \param forceMode Determines how the torque is going to be applied.
        virtual void addTorque(const physx::PxVec3& torque, const physx::PxForceMode::Enum forceMode)
        {
            if (auto* rigidDynamic = rigidBody->is<physx::PxRigidDynamic>())
                rigidDynamic->addTorque(torque, forceMode);
        }

        /// \brief Clears specified force.
        /// \param forceMode The mode to use when clearing the force.
        void clearForce(const physx::PxForceMode::Enum forceMode)
        {
            if (auto* rigidDynamic = rigidBody->is<physx::PxRigidDynamic>())
                rigidDynamic->clearForce(forceMode);
        }

        /// \brief Clears specified torque.
        /// \param forceMode The mode to use when clearing the torque.
        void clearTorque(const physx::PxForceMode::Enum forceMode)
        {
            if (auto* rigidDynamic = rigidBody->is<physx::PxRigidDynamic>())
                rigidDynamic->clearTorque(forceMode);
        }

        /// \brief Clears all forces.
        void clearAllForces()
        {
            if (auto* const rigidDynamic = rigidBody->is<physx::PxRigidDynamic>())
            {
                rigidDynamic->setLinearVelocity(physx::PxVec3(0, 0, 0));
                rigidDynamic->setAngularVelocity(physx::PxVec3(0, 0, 0));

                rigidDynamic->clearForce();
                rigidDynamic->clearForce(physx::PxForceMode::eIMPULSE);
                rigidDynamic->clearForce(physx::PxForceMode::eACCELERATION);
                rigidDynamic->clearForce(physx::PxForceMode::eVELOCITY_CHANGE);

                rigidDynamic->clearTorque();
                rigidDynamic->clearTorque(physx::PxForceMode::eIMPULSE);
                rigidDynamic->clearTorque(physx::PxForceMode::eACCELERATION);
                rigidDynamic->clearTorque(physx::PxForceMode::eVELOCITY_CHANGE);
            }
        }

        [[nodiscard]] const glm::mat4& getTransform() const
        {
            return transform;
        }

        [[nodiscard]] bool isActive() const
        {
            return bIsActive;
        }

        [[nodiscard]] physx::PxScene* getPxScene() const
        {
            return pxScene;
        }

        [[nodiscard]] physx::PxRigidActor* getRigidBody() const
        {
            return rigidBody;
        }

        [[nodiscard]] physx::PxShape* getShape() const
        {
            return shape;
        }

    protected:
        /// \brief Initialize physics.
        /// Override to initialize your physics here.
        /// \param initTransform Initial transform.
        virtual void initPhysics(const glm::mat4& initTransform)
        {
            bPhysicsInitialized = true;

            CPhysicsComponent::transform = initTransform;
        }

        /// \brief Called by the physics subsystem when physics got stepped.
        /// Override to react to physics simulation steps.
        /// \param initTransform Initial transform.
        virtual void stepPhysics(const float stepSize)
        {
            event_onPhysicsStep.fire();
            event_onPhysicsStepEnd.fire();
        }

        /// \brief Adds the physics body to the world.
        virtual void addToWorld()
        {
            if (pxScene)
            {
                if (rigidBody)
                    pxScene->addActor(*rigidBody);
            }
        }

        /// \brief Removes from the physics body from the world.
        virtual void removeFromWorld()
        {
            if (pxScene)
            {
                if (rigidBody)
                    pxScene->removeActor(*rigidBody);
            }
        }

        /// \brief Clean up.
        virtual void destroyPhysics()
        {
            if (bIsActive)
            {
                deactivate();
                rigidBody->release();
                rigidBody = nullptr;
                shape = nullptr;
                bPhysicsInitialized = false;
            }
        }

        /// \brief Used to call the event_rigidActor_t from the child class.
        /// \param event The event that should fire.
        void fireRigidActorEvent(event_rigidActor_t* const event)
        {
            event->fire(rigidBody);
        }

        /// \brief Used to call the event_rigidActor_t from the child class.
        /// \param event The event that should fire.
        static void fireFloatEvent(event_float_t* const event, const float payload)
        {
            event->fire(payload);
        }

        /// \brief Used to call the event_rigidActor_t from the child class.
        /// \param event The event that should fire.
        static void fireVoidEvent(event_void_t* const event)
        {
            event->fire();
        }

    public:
        event_rigidActor_t event_onRigidBodyCreated;

        event_void_t event_onPhysicsStep;

        event_void_t event_onPhysicsStepEnd;

        event_void_t event_deactivated;

    protected:
        physx::PxPhysics* pxPhysics;

        physx::PxScene* pxScene;

        const physx::PxMaterial* pxDefaultMat;

        glm::mat4 transform;

        physx::PxRigidActor* rigidBody;

        physx::PxShape* shape;

        bool bNeedsUpdate;

        bool bIsActive;

        bool bPhysicsInitialized;
    };
}