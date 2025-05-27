// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>
// Extended the by Cem Özdemir
#pragma once
#include <engine/callbacks/Event.h>
#include <engine/Engine.h>
#include <engine/entities/Entity.h>

namespace engine::world
{
    class World
    {
    public:
        /* Events */
        using world_tick_t = typename callbacks::Event<World, float>;
        using key_input_event_t = typename callbacks::Event<World, const GLFWwindow*, int, int, int, int>;
        using controller_input_event_t = typename callbacks::Event<World, int, GLFWgamepadstate>;
        using mouse_move_event_t = typename callbacks::Event<World, const double, const double>;
        using mouse_input_event_t = typename callbacks::Event<World, const int, const int, const int>;
        using mouse_scroll_event_t = typename callbacks::Event<World, const double, const double>;
        using mouse_catch_event_t = typename callbacks::Event<World, GLFWwindow*, const bool>;

        explicit World(engine::Engine* engine);

        ~World();

        entities::Entity createEntity();

        template <typename A, typename... Params>
        std::shared_ptr<A> createActor(Params&& ... params)
        {
            std::shared_ptr<A> actor = std::make_shared<A>(std::forward<Params>(params) ...);
            actors.emplace_back(actor);
            return actor;
        }

        void destroyActor(AActor* actor);

        void destroyEntity(entities::Entity entity);

    private:
        void shutdown();

        void destroyMarkedActors();

    public:
        world_tick_t event_worldTick;

        engine::Engine::tick_event_t::handle_t handle_engineTick;

        engine::context::Context::event_void_t::handle_t handle_engineEndFrame{};

        key_input_event_t event_keyInput;

        engine::context::Context::key_input_event_t::handle_t handle_contextKeyInput;

        controller_input_event_t event_controllerInput;

        engine::context::Context::controller_input_event_t::handle_t handle_contextControllerInput;

        mouse_move_event_t event_mouseMove;

        engine::context::Context::mouse_move_event_t::handle_t handle_contextMouseMove;

        mouse_input_event_t event_mouseInput;

        engine::context::Context::mouse_input_event_t ::handle_t handle_contextMouseInput;

        mouse_scroll_event_t event_mouseScroll;

        engine::context::Context::mouse_scroll_event_t::handle_t handle_contextMouseScroll;

        mouse_catch_event_t event_mouseCatch;

        engine::context::Context::mouse_catch_event_t ::handle_t handle_contextMouseCatch;

    private:
        using actor_container_t = typename std::vector<std::shared_ptr<engine::world::AActor>>;
        actor_container_t actors;

        using actor_ptr_container_t = typename std::vector<engine::world::AActor*>;
        actor_ptr_container_t actorsToDestroy;

        Engine* engine;
    };
}

