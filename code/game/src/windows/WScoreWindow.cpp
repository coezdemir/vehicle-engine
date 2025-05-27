//
// Created by CemOe on 16.01.2022.
//

#include "WScoreWindow.h"
#include "../world/ACar.h"
#include "../gamemode/PointGamemode.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT
#include <nuklear.h>

game::ui::windows::WScoreWindow::WScoreWindow(nk_context** nkContext, GLFWwindow* glfwWindow, world::ACar* player,
                                              game::gamemode::PointGamemode* pointGamemode)
        : nkContext(nkContext), glfwWindow(glfwWindow), player(player), pointGamemode(pointGamemode)
{
    score = 0;
}

void game::ui::windows::WScoreWindow::init()
{
    score = pointGamemode->getScore(player);
    numberOfViewports = pointGamemode->getNumberOfPlayers();
    id = player->getId();
    glfwGetWindowSize(glfwWindow, &glfwWindowWidth, &glfwWindowHeight);
    windowPosX = id == 0 || id == 2 ? 0.f + 10.f : (glfwWindowWidth / 2) + 10.f;
    windowPosY = id == 0 || id == 1 ? (glfwWindowHeight / 2) - 100.f : glfwWindowHeight - 100.f;
    windowWidth = numberOfViewports < 3 ? 300.f : 200.f;
    windowHeight = numberOfViewports < 3 ? 100.f : 75.f;
}

void game::ui::windows::WScoreWindow::draw()
{
    init();

    if (nk_begin(*nkContext, ("    Score Player " + std::to_string(id + 1)).c_str(),
                 nk_rect(windowPosX, windowPosY, windowWidth, windowHeight),
                 NK_WINDOW_BORDER | NK_WINDOW_TITLE))
    {
        nk_layout_row_begin(*nkContext, NK_DYNAMIC, 10, 2);
        {
            nk_layout_row_push(*nkContext, 0.05f);
            nk_label(*nkContext, "", NK_TEXT_ALIGN_LEFT);

            nk_layout_row_push(*nkContext, 0.95f);
            nk_label(*nkContext, std::to_string(score).c_str(), NK_TEXT_ALIGN_LEFT);
        }
        nk_layout_row_end(*nkContext);
    }
    nk_end(*nkContext);
}


