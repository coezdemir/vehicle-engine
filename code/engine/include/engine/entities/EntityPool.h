// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>
#pragma once
#include "../callbacks/Event.h"
#include "../Engine_NS.h"
#include "Entity.h"
#include <functional>
#include <set>
#include <stdexcept>
#include <vector>

namespace engine::entities
{
    class EntityPool
    {
        /* Events */
        using event_t = typename callbacks::Event<EntityPool, Entity>;

    public:
        explicit EntityPool(engine::Engine* engine);

        EntityPool(EntityPool &&) = delete;

        EntityPool(const EntityPool &) = delete;

        Entity getEntity(id_t id);

        bool isAlive(id_t id);

        Entity createEntity();

        void releaseEntity(Entity entity);

        event_t event_destroyedEntity;

        event_t event_createdEntity;

        event_t event_destroyed;

    private:
        id_t next_id = {0};

        std::set<id_t> activeEntities;

        engine::Engine* engine;
    };
}
