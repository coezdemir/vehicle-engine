// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>
#pragma once
#include <engine/components/ComponentPool.h>
#include <glm/glm.hpp>

namespace engine::entities
{
    struct Entity final
    {
        friend class EntityPool;

    public:
        Entity(Entity &&) = default;

        Entity(Entity const &) = default;

        template<typename C>
        typename components::component_pool_t<C>::handle_t addComponent();

        template<typename C>
        typename components::component_pool_t<C>::handle_t getComponent(components::id_t id);

        template<typename C>
        typename components::component_pool_t<C>::handle_t getComponent();

        template<typename C>
        typename components::component_pool_t<C>::index_set_t getComponents();

        template <typename C>
        void removeComponent(typename components::id_t id);

        [[nodiscard]] bool isValid() const;
    private:
        explicit Entity(EntityPool* pool, engine::Engine* engine, id_t id);

    public:
        const engine::entities::id_t id;

    private:
        EntityPool* pool = nullptr;

        engine::Engine* engine = nullptr;
    };
}