//
// Created by CemOe on 16.01.2022.
//
#pragma once
#include <engine/ui/windows/WWindow.h>

struct nk_context;
enum EItem;

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
        class WItemWindow : public engine::ui::windows::WWindow
        {
        public:
            explicit WItemWindow(nk_context** nkContext, GLFWwindow* window, world::ACar* player,
                                 game::gamemode::PointGamemode* pointGamemode);

            void draw() override;

            void init() override;

        private:
            std::string getItemString(EItem itemToString);

            nk_context** nkContext;

            GLFWwindow* glfwWindow;

            gamemode::PointGamemode* pointGamemode;

            world::ACar* player;

            EItem item;

            float windowWidth, windowHeight, windowPosX, windowPosY;

            int glfwWindowWidth, glfwWindowHeight;

            int numberOfViewports;

            int id;
        };
    }
}