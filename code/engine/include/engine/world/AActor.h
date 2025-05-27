// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>
#pragma once
#include <engine/entities/Entity.h>
#include <engine/world/World.h>

namespace engine::world
{
    class AActor
    {
    public:
        friend World;

        explicit AActor(World* world);

        virtual void tick(float deltaTime);

        engine::entities::Entity getRootEntity()
        {
            return rootEntity;
        }

        virtual void destroy()
        {
            if(bIsAlive)
            {
                world->event_worldTick.unsubscribe(handle_worldTick);

                world->destroyActor(this);

                bIsAlive = false;
            }
        }

        bool isAlive() const;

    protected:
        engine::entities::Entity rootEntity;

        World::world_tick_t::handle_t handle_worldTick;

        World* const world;

        bool bIsAlive;
    };
}