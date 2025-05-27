//
// Created by Cem on 20.07.2022.
//
#pragma once

#include <engine/ui/windows/WWindow.h>

struct nk_context;

namespace game
{
    namespace actors
    {
        class ACar;
    }

    namespace gamemode
    {
        class PointGamemode;
    }

    namespace ui::windows
    {
        class WCircuitEnd : public engine::ui::windows::WWindow
        {
        public:
            explicit WCircuitEnd(nk_context** nkContext, GLFWwindow* window, std::vector<int>  pointMap);

            void draw() override;

            void init() override;

            using event_t = typename engine::callbacks::Event<WCircuitEnd>;
            event_t event_continue;

        private:
            nk_context** nkContext;

            GLFWwindow* glfwWindow;

            std::pair<int, int> winner;

            std::vector<int> points;
        };
    }
}
