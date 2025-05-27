//
// Created by Cem on 22.04.2022.
//
#pragma once
#include <utility>

#include "MyCollisionCallback.h"
#include "../world/ACar.h"

namespace game::physics::collision
{
    class MyCollisionResponse
    {
    public:
        MyCollisionResponse() = default;

        MyCollisionResponse(MyCollisionCallback* collisionCallback, std::vector<game::world::ACar*> vehicles)
            : collisionCallback(collisionCallback), vehicles(std::move(vehicles))
        {
            collision_handle = collisionCallback->event_collision.subscribe([this]
            (engine::physics::components::CPhysicsComponent* comp1, engine::physics::components::CPhysicsComponent* comp2){
                handleCollision(comp1, comp2);
            });
        }

        MyCollisionResponse& operator=(MyCollisionResponse&& other) noexcept
        {
            if (this != &other)
            {
                // Unsubscribe from events
                other.collisionCallback->event_collision.unsubscribe(other.collision_handle);
                collisionCallback = other.collisionCallback;

                //Resubscribe
                collision_handle = collisionCallback->event_collision.subscribe([this]
                (engine::physics::components::CPhysicsComponent* comp1, engine::physics::components::CPhysicsComponent* comp2){
                    handleCollision(comp1, comp2);
                });


                vehicles = other.vehicles;

                return *this;
            }

            return *this;;
        }

    private:
        void handleCollision(engine::physics::components::CPhysicsComponent* component1, engine::physics::components::CPhysicsComponent* component2)
        {
            float knockBack = 75.f;
            bool firstCarFound = false;
            bool secondCarFound = false;

            for (auto& vehicle : vehicles)
            {
                if(!firstCarFound && vehicle == component1->getInstantiator())
                    firstCarFound = true;
                else if(!secondCarFound && vehicle == component2->getInstantiator())
                    secondCarFound = true;
            }

            if (firstCarFound && secondCarFound)
            {
                PxVec3 car1Pos = component1->getRigidBody()->getGlobalPose().p;
                PxVec3 car2Pos = component2->getRigidBody()->getGlobalPose().p;
                PxVec3 impulseDir1 = car2Pos - car1Pos;
                PxVec3 impulseDir2 = car1Pos - car2Pos;
                impulseDir1.normalize();
                impulseDir2.normalize();

                modifyKnockback(component1->getInstantiator(), knockBack);
                applyKnockback(component2, component1, knockBack, impulseDir1);

                modifyKnockback(component2->getInstantiator(), knockBack);
                applyKnockback(component1, component2, knockBack, impulseDir2);
            }

            if (firstCarFound)
            {
                auto* rigidDynamic = component1->getRigidBody()->is<PxRigidDynamic>();
                PxVec3 velocity = rigidDynamic->getLinearVelocity();
                velocity.y = 0;
                velocity.normalize();

                applyKnockback(component1, nullptr, knockBack, -velocity);
            }
            else if (secondCarFound)
            {
                auto* rigidDynamic = component2->getRigidBody()->is<PxRigidDynamic>();
                PxVec3 velocity = rigidDynamic->getLinearVelocity();
                velocity.y = 0;
                velocity.normalize();

                applyKnockback(component2, nullptr, knockBack, -velocity);;
            }
        }

        void modifyKnockback(engine::world::AActor* actor, float& knockback)
        {
            knockback = 75.f;

            game::world::ACar* car = dynamic_cast<game::world::ACar*>(actor);
            ECarState carState = car->getState();

            if (carState == ECarState::KNOCKBACK_INCREASE)
                knockback *= 2.f;
        }

        void applyKnockback(engine::physics::components::CPhysicsComponent* comp, engine::physics::components::CPhysicsComponent* otherComp, float& knockback, const PxVec3& direction)
        {
            game::world::ACar* car = dynamic_cast<game::world::ACar*>(comp->getInstantiator());

            if(car->collided())
                return;

            ECarState carState = car->getState();

            if (carState == ECarState::KNOCKBACK_DECREASE)
                knockback *= 0.25f;

            // Tell the car to bounce after tick finishes
            car->setCollided(true, direction * knockback, otherComp ? dynamic_cast<game::world::ACar*>(otherComp->getInstantiator()) : nullptr);
        }

    private:
        std::vector<game::world::ACar*> vehicles;

        MyCollisionCallback* collisionCallback;

        MyCollisionCallback::event_collision_t::handle_t collision_handle;
    };
}