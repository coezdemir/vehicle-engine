//
// Created by Cem on 06.01.2022.
//
#include "WWindow.h"

namespace engine::ui::windows
{
    /// \brief Nuklear GUI Demo
    class WNuklearDemo : public WWindow
    {
    public:
        WNuklearDemo();

        explicit WNuklearDemo(GLFWwindow* window, struct nk_context** nkContext);

        void draw() override;

    private:
        void init() override;

    public:
        GLFWwindow* window;

    private:
        struct nk_context** nkContext;
    };
}