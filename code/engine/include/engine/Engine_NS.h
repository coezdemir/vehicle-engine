//
// Created by CemOe on 14.08.2021.
//
#pragma once

namespace engine
{
    class Engine;

    namespace world
    {
        class AActor;

        class World;
    }

    namespace entities
    {
        struct id_t;

        struct Entity;

        class EntityPool;
    }

    namespace components
    {
        struct id_t;
        struct CComponent;

        class ComponentContainer;

        template <typename, size_t>
        class ComponentPool;

        template <typename C>
        using component_pool_t = typename engine::components::ComponentPool<C, 4096>;
    }
}

#define _TYPESAFE_ID_TYPE_NAME engine::entities::id_t
#include "internal/typesafe_id.h"
#undef _TYPESAFE_ID_TYPE_NAME

#define _TYPESAFE_ID_TYPE_NAME engine::components::id_t
#include "internal/typesafe_id.h"
#undef _TYPESAFE_ID_TYPE_NAME

namespace engine::entities
{
    const engine::entities::id_t invalid_id{id_t::invalid_value};
}

namespace engine::components
{
    const engine::components::id_t invalid_id{id_t::invalid_value};
}
