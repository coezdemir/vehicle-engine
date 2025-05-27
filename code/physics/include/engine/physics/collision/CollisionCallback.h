//
// Created by Cem on 21.04.2022.
//
#pragma once
#include "engine/physics/components/collision/CCollision.h"

namespace engine::physics::collision
{
    /// \brief Callback from the physics engine that allows us to react to certain event.
    /// Override this if u want custom behaviour for various physics events.
    /// For more information take a look the the PhysX 4.1 documentation:
    /// https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Manual/Simulation.html?highlight=callback
    class CollisionCallback : public physx::PxSimulationEventCallback
    {
    public:
        CollisionCallback() = default;

        virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override
        {
            // ignore pairs when one actor has been deleted
            if(pairHeader.flags.isSet(physx::PxContactPairHeaderFlag::eREMOVED_ACTOR_0) ||
               pairHeader.flags.isSet(physx::PxContactPairHeaderFlag::eREMOVED_ACTOR_1))
                return;

            // ignore pairs when one shape has been deleted
            if (pairs->flags.isSet(physx::PxContactPairFlag::eREMOVED_SHAPE_0) ||
                pairs->flags.isSet(physx::PxContactPairFlag::eREMOVED_SHAPE_1))
                return;

            auto* component1 =
                    reinterpret_cast<engine::physics::components::CPhysicsComponent*>(pairHeader.actors[0]->userData);

            auto* component2 =
                    reinterpret_cast<engine::physics::components::CPhysicsComponent*>(pairHeader.actors[1]->userData);

            if(component1 && component2)
            {
                if (auto* cComponent = dynamic_cast<engine::physics::components::CCollision*>(component1))
                {
                    engine::physics::components::CollisionInfo collisionInfo{};
                    collisionInfo.otherActor = component2->getInstantiator();
                    collisionInfo.otherComponent = component2;
                    collisionInfo.otherBody = component2->getRigidBody();
                    collisionInfo.otherShape = component2->getShape();

                    cComponent->fireCollisionEvent(&cComponent->event_onHit, collisionInfo);
                }

                if (auto* cComponent = dynamic_cast<engine::physics::components::CCollision*>(component2))
                {
                    engine::physics::components::CollisionInfo collisionInfo{};
                    collisionInfo.otherActor = component1->getInstantiator();
                    collisionInfo.otherComponent = component1;
                    collisionInfo.otherBody = component1->getRigidBody();
                    collisionInfo.otherShape = component1->getShape();

                    cComponent->fireCollisionEvent(&cComponent->event_onHit, collisionInfo);
                }
            }
        }

        void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override
        {
            for (physx::PxU32 i = 0; i < count; i++)
            {
                // ignore pairs when shapes have been deleted
                if (pairs[i].flags & (physx::PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER |
                                      physx::PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
                    continue;


                auto* triggerComp =
                        reinterpret_cast<engine::physics::components::CCollision*>(pairs->triggerActor->userData);

                auto* otherComp =
                        reinterpret_cast<engine::physics::components::CPhysicsComponent*>(pairs->otherActor->userData);

                if(triggerComp && otherComp)
                {
                    engine::physics::components::CollisionInfo collisionInfo{};
                    collisionInfo.otherActor = otherComp->getInstantiator();
                    collisionInfo.otherComponent = otherComp;
                    collisionInfo.otherBody = otherComp->getRigidBody();
                    collisionInfo.otherShape =  otherComp->getShape();

                    triggerComp->fireCollisionEvent(&triggerComp->event_onOverlap, collisionInfo);
                }
            }
        }
        void onConstraintBreak(physx::PxConstraintInfo *constraints, physx::PxU32 count) override
        {

        }

        void onWake(physx::PxActor **actors, physx::PxU32 count) override
        {

        }

        void onSleep(physx::PxActor **actors, physx::PxU32 count) override
        {

        }

        void onAdvance(const physx::PxRigidBody *const *bodyBuffer, const physx::PxTransform *poseBuffer,
                       const physx::PxU32 count) override
        {

        }
    };
}