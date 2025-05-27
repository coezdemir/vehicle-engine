// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>
#pragma once
#include <engine/components/ComponentPool.h>
#include "engine/context/Context.h"
#include <engine/entities/EntityPool.h>
#include <engine/memory/LinearArray.h>
#include <engine/system/System.h>
#include <engine/callbacks/Event.h>
#include <engine/Engine_NS.h>
#include <algorithm>
#include <map>
#include <typeindex>
#include <vector>
#include <engine/entities/Entity.h>

namespace engine
{
    class Engine
    {
    public:
        friend class engine::world::World;

        /* Events */
        using event_engineptr_t = typename callbacks::Event<Engine, Engine*>;
        using tick_event_t = typename callbacks::Event<Engine, const float>;
        using event_void_t = typename callbacks::Event<Engine>;

        Engine();

        void printInstance();

        void run();

        context::Context& getContext();

        /***
         * Returns a pointer to an existing component container, if it exists, or throws an exception otherwise.
         * Pointers are guaranteed not to change over the lifetime of the application.
         * @tparam C Component Type
         * @return Pointer to the (unique) container for this type of component. Throws exception if not found.
         */
        template <typename C>
        components::component_pool_t<C>* getContainer() const
        {
            using container_t = typename components::component_pool_t<C>;
            return dynamic_cast<container_t*>(componentContainers.at(std::type_index(typeid(C))).get());
        }

        /***
         * Creates a new component container for a component. Returns an existing instance, if available.
         * Should not be used after initialization phase.
         * @tparam C The component type to be stored.
         * @return A pointer to the component container for the specified type.
         */
        template <typename C>
        components::component_pool_t<C>* createContainer()
        {
            using container_t = typename components::component_pool_t<C>;
            auto index = std::type_index(typeid(C));

            // Check if container already exists
            auto c = componentContainers.find(index);
            if (c != componentContainers.end())
                return dynamic_cast<container_t*>(c->second.get());

            // Create container
            auto* container = new components::component_pool_t<C>();
            componentContainers[index].reset(static_cast<components::ComponentContainer*>(container));
            printf("-- New component container has been created.\n");

            // We ignore the event handle until deleting containers is implemented
            entities.event_destroyedEntity.subscribe([container](auto entity) {
                container->eraseAll(entity.id);
            });

            // Erases elements that are marked as 'to delete'
            context.event_endFrame.subscribe([container]() {
                container->eraseImmediately();
            });

            return container;
        }

        /***
         * Create a system of type T to this instance of the engine.
         * The system is registered after the call finishes, but not started yet.
         * @tparam T the type of the system
         * @tparam Args the type of the constructor arguments for T.
         * @param args the arguments for the constructor of T.
         * @return A pointer to the system instance that will not change over the lifetime of the engine instance.
         */
        template <typename T, typename ... Args>
        T* addSystem(Args&& ... args)
        {
            systems.add(std::make_unique<T>(std::forward<Args>(args)...));
            auto* s = systems[systems.count() - 1].get();
            s->registerSystem(this);

            printf("-- New system has been added.\n");
            return static_cast<T*>(s);
        }

        /***
         * Get the first instance of a system of type S added using addSystem().
         * @tparam S the type of the system.
         * @return a pointer to the first available system of type S or nullptr if none exists.
         */
        template <typename S>
        S* getSystem() const
        {
            for (const auto& s_ptr : systems)
            {
                S* s = dynamic_cast<S*>(s_ptr.get());
                if (s != nullptr)
                    return s;
            }

            return nullptr;
        }

        entities::Entity getEntity(entities::id_t id);

        event_engineptr_t event_startup;

        event_engineptr_t event_afterStartup;

        event_engineptr_t event_beginPlay;

        event_engineptr_t event_beforeShutdown;

        event_engineptr_t event_shutdown;

        tick_event_t event_tick;

        event_void_t event_draw;

        event_void_t event_newFrame;

        event_void_t event_endFrame;

    private:
        void tick(context::Context* c);

        context::Context context = engine::context::Context();

        engine::entities::EntityPool entities = entities::EntityPool(this);

        using component_container_map_t = std::map<std::type_index, std::unique_ptr<components::ComponentContainer>>;
        component_container_map_t componentContainers;

        using system_container_t = memory::LinearArray<std::unique_ptr<system::System>, 128>;
        system_container_t systems;

        const bool SKIP_FIRST_TICK = true;

        bool bSkippedFirstTick;
    };

    template <typename C>
    typename engine::components::component_pool_t<C>::handle_t engine::entities::Entity::addComponent()
    {
        return engine->getContainer<C>()->create(id);
    }

    template <typename C>
    typename engine::components::component_pool_t<C>::handle_t engine::entities::Entity::getComponent(components::id_t componentID)
    {
        return engine->getContainer<C>()->get(componentID);
    }

    template <typename C>
    typename engine::components::component_pool_t<C>::index_set_t engine::entities::Entity::getComponents()
    {
        return engine->getContainer<C>()->getAll(id);
    }

    template <typename C>
    void engine::entities::Entity::removeComponent(components::id_t componentID)
    {
        engine->getContainer<C>()->erase(engine->getContainer<C>()->get(componentID));
    }

    template <typename C>
    typename engine::components::component_pool_t<C>::handle_t entities::Entity::getComponent()
    {
        return engine->getContainer<C>()->getAny(id);
    }
}
