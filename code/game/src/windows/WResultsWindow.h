//
// Created by CemOe on 16.01.2022.
//
#pragma once
#include <engine/ui/windows/WWindow.h>

class ACar;

struct nk_context;

namespace game::ui::windows
{
    class WResultsWindow : public engine::ui::windows::WWindow
    {
    public:
        explicit WResultsWindow(nk_context** nkContext, GLFWwindow* window, std::vector<int> pointMap);

        void init() override;

        void draw() override;

        using event_t = typename engine::callbacks::Event<WResultsWindow>;
        event_t event_continue;

    private:
        nk_context** nkContext;

        GLFWwindow* glfwWindow;

        std::vector<int> points;
    };
}