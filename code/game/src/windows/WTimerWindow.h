//
// Created by CemOe on 21.01.2022.
//

#pragma once
#include <engine/ui/windows/WWindow.h>

class Car;

namespace game
{
    namespace world
    {
        class ACar;
    }

    namespace gamemode
    {
        class PointGamemode;
    }

    namespace ui::windows
    {
        class WTimerWindow : public engine::ui::windows::WWindow
        {
        public:
            explicit WTimerWindow(nk_context** nkContext, GLFWwindow* window, world::ACar* player, game::gamemode::PointGamemode* pointGamemode);

            void init() override;

            void draw() override;

        private:
            nk_context** nkContext;

            GLFWwindow* glfwWindow;

            world::ACar* player;

            gamemode::PointGamemode* pointGamemode;

            int glfwWindowWidth, glfwWindowHeight;

            int id;

            float windowPosX, windowPosY;

            float windowWidth, windowHeight;
        };
    }
}

