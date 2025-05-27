//
// Created by CemOe on 27.09.2021.
//
#pragma once
#include <engine/system/System.h>
#include <engine/Engine.h>

namespace engine::ui::windows
{
    class WWindow;
}

struct nk_context;
struct nk_font_atlas;

namespace engine::ui
{
    /// \brief Subsystem that manages ui windows and draws them.
    class UserInterface final : public engine::system::System
    {
    public:
        using skin_nuklear_event_t = typename callbacks::Event<UserInterface, struct nk_context*, struct NuklearGUI*>;

        UserInterface();

        UserInterface(UserInterface&&) = delete;

        UserInterface(const UserInterface&) = delete;

        /// \brief Construct a new window and add it to the draw list.
        /// \tparam T Window class
        /// \tparam Args Constructor arguments classes
        /// \param args Constructor arguments
        /// \return shared_ptr for the constructed window
        template <typename T, typename ... Args>
        std::shared_ptr<windows::WWindow> addWindow(Args&& ... args)
        {
            windows.add(std::make_shared<T>(std::forward<Args>(args)...));
            registerWindow(windows[windows.count() - 1]);

            printf("-- New window has been added.\n");
            return windows[windows.count() - 1];
        }

        /// Removes a window from the drawing list but does not destroy it.
        /// \param window shared_ptr to remove from the drawing list.
        /// \return Success
        bool removeWindowFromDrawList(const std::shared_ptr<windows::WWindow>& window);

        /// Readd a removed window to the drawing list.
        /// \param window shared_ptr to readd to the drawing list
        /// \return Success
        bool readdWindowToDrawList(const std::shared_ptr<windows::WWindow>& window);

    private:
        /// Calls init on the window
        /// \param window
        void registerWindow(const std::shared_ptr<windows::WWindow>& window);

        /// \brief Registers the system to the engine.
        /// \param engine Pointer to the engine.
        void registerSystem(Engine* engine) override;

        void startup(Engine* pEngine);

        void tick(float deltaTime);

        void shutdown(Engine* engine);

        void newFrame();

        void endFrame();

        void draw();

        /// \brief Skin the nuklear gui with the skin at "assets/textures/gui/nuklear-skin.png"
        void skinNuklear();

        Engine::event_engineptr_t::handle_t handle_startup;

        Engine::tick_event_t::handle_t handle_tick;

        context::Context::event_void_t::handle_t handle_endFrame;

        Engine::event_engineptr_t::handle_t handle_shutdown;

        Engine::event_void_t::handle_t handle_newFrame;

        Engine::event_void_t::handle_t handle_draw;

        memory::LinearArray<std::shared_ptr<windows::WWindow>, 16> windows;

        std::vector<std::shared_ptr<windows::WWindow>> windowsToRemove;

        static struct NuklearGUI nuklearGUI;

        static nk_font_atlas atlas;

        unsigned int nuklearSkinTexture;

        bool bShowDemo;

        bool bShowDebugOptions;

    public:
        struct nk_context* nkContext;

        skin_nuklear_event_t event_skinNuklear;
    };
}