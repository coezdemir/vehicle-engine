//
// Created by CemOe on 16.01.2022.
//
#pragma once
#include <engine/ui/windows/WWindow.h>

struct nk_context;

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
        class WScoreWindow : public engine::ui::windows::WWindow
        {
        public:
            explicit WScoreWindow(nk_context** nkContext, GLFWwindow* window, world::ACar* player, gamemode::PointGamemode* pointGamemode);

            void draw() override;

            void init() override;

        private:
            int id;

            nk_context** nkContext;

            GLFWwindow* glfwWindow;

            world::ACar* player;

            gamemode::PointGamemode* pointGamemode;

            int score;

            float windowWidth, windowHeight, windowPosX, windowPosY;

            int glfwWindowWidth, glfwWindowHeight;

            int numberOfViewports;
        };
    }
}