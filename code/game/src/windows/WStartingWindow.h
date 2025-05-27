//
// Created by CemOe on 11.01.2022.
//
#include <engine/ui/windows/WWindow.h>

namespace game::ui::windows
{
    class WStartingWindow : public engine::ui::windows::WWindow
    {
    public:
        explicit WStartingWindow(struct nk_context** nkContext, GLFWwindow* window);

        void draw() override;

    private:
        struct nk_context** nkContext;

        void init() override;

        GLFWwindow* window;

        int glfwWindowWidth, glfwWindowHeight;

    public:
        using event_t = typename engine::callbacks::Event<WStartingWindow>;
        event_t event_continue;
    };
}
