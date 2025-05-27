//
// Created by CemOe on 21.01.2022.
//

#include "WTimerWindow.h"
#include "../world/ACar.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT
#include <nuklear.h>

game::ui::windows::WTimerWindow::WTimerWindow(nk_context** nkContext, GLFWwindow* window, world::ACar* player,
                                              game::gamemode::PointGamemode* pointGamemode)
                                            : nkContext(nkContext), glfwWindow(window), player(player),
                                            pointGamemode(pointGamemode)
{

}

void game::ui::windows::WTimerWindow::init()
{
    int lastglfwWindowWidth, lastglfwWindowHeight;
    lastglfwWindowWidth = glfwWindowWidth;
    lastglfwWindowHeight = glfwWindowHeight;
    glfwGetWindowSize(glfwWindow, &glfwWindowWidth, &glfwWindowHeight);

    if(lastglfwWindowWidth == glfwWindowWidth && lastglfwWindowHeight == glfwWindowHeight)
        return;

    windowWidth = 100.f;
    windowHeight = 74.f;
    id = player->getId();
    windowPosX = id == 0 || id == 2 ? 0 + 10.f : (glfwWindowWidth / 2) + 10.f;
    windowPosY = id == 0 || id == 1 ? 0 + 10.f : (glfwWindowHeight / 2) + 10.f;
}

void game::ui::windows::WTimerWindow::draw()
{
    init();

    std::string windowTitle = "Time left: ";

    for(int i = 0; i < id + 1; i++)
        windowTitle.append(" ");

    if (nk_begin(*nkContext, windowTitle.c_str(),
                 nk_rect(windowPosX, windowPosY, windowWidth, windowHeight),
                 NK_WINDOW_BORDER | NK_WINDOW_TITLE))
    {
        nk_layout_row_begin(*nkContext, NK_DYNAMIC, 12, 1);
        {
            nk_layout_row_push(*nkContext, 1.f);
            nk_label(*nkContext, std::to_string((int) pointGamemode->getTime()).c_str(), NK_TEXT_ALIGN_CENTERED);
        }
    }
    nk_end(*nkContext);
}
