//
// Created by Cem on 20.04.2022.
//
#pragma once

#include <engine/physics/collision/CollisionCallback.h>
#include <engine/callbacks/Event.h>

using namespace physx;

namespace game::physics::collision
{
    struct FilterGroup
    {
        enum Enum
        {
            eVEHICLE = (1 << 0),
            eOBSTACLE = (1 << 1),
            eGHOST = (1 << 2)
        };

        static void setupFiltering(const PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask,
                                   PxU32 customWord2 = INVALID_FILTER_PAIR_INDEX,
                                   PxU32 customWord3 = INVALID_FILTER_PAIR_INDEX)
        {
            PxFilterData filterData;
            filterData.word0 = filterGroup; // word0 = own ID
            filterData.word1 = filterMask;  // word1 = ID mask to filter pairs that trigger a contact callback;
            filterData.word2 = customWord2;
            filterData.word3 = customWord3;
            const PxU32 numShapes = actor->getNbShapes();
            PxShape** shapes = (PxShape**) malloc(sizeof(PxShape*) * numShapes);
            actor->getShapes(shapes, numShapes);
            for (PxU32 i = 0; i < numShapes; ++i)
            {
                PxShape* shape = shapes[i];
                shape->setSimulationFilterData(filterData);
            }
            free(shapes);
        }

        static PxFilterFlags GameFilterShader(
                PxFilterObjectAttributes attributes0, PxFilterData filterData0,
                PxFilterObjectAttributes attributes1, PxFilterData filterData1,
                PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
        {
            // let triggers through
            if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
            {
                pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
                return PxFilterFlag::eDEFAULT;
            }

            if(filterData0.word0 == eGHOST || filterData1.word0 == eGHOST)
                return PxFilterFlag::eSUPPRESS;

            // generate contacts for all that were not filtered above
            pairFlags = PxPairFlag::eCONTACT_DEFAULT;

            // trigger the contact callback for pairs (A,B) where
            // the filtermask of A contains the ID of B and vice versa.
            if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
                pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

            if(filterData0.word0 == eVEHICLE && filterData1.word0 == eVEHICLE)
                pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

            return PxFilterFlag::eDEFAULT;
        }
    };


    class MyCollisionCallback : public engine::physics::collision::CollisionCallback
    {
    public:
        MyCollisionCallback() = default;

        void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override
        {
            CollisionCallback::onContact(pairHeader, pairs, nbPairs);

            for (PxU32 i = 0; i < nbPairs; i++)
            {
                const PxContactPair& cp = pairs[i];

                // Due to our filters this if statement only passes, when a collision involving our vehicle occurs
                if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
                {
                    engine::physics::components::CPhysicsComponent* component1;
                    engine::physics::components::CPhysicsComponent* component2;

                    component1 =
                            reinterpret_cast<engine::physics::components::CPhysicsComponent*>(pairHeader.actors[0]->userData);

                    component2 =
                            reinterpret_cast<engine::physics::components::CPhysicsComponent*>(pairHeader.actors[1]->userData);

                    event_collision.fire(component1, component2);
                }
            }
        }

        void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer,
                       const physx::PxU32 count) override
        {
            CollisionCallback::onAdvance(bodyBuffer, poseBuffer, count);

        }

        using event_collision_t = typename engine::callbacks::Event<MyCollisionCallback,engine::physics::components::CPhysicsComponent*, engine::physics::components::CPhysicsComponent*>;
        event_collision_t event_collision;
    };
}

