//
// Created by CemOe on 16.01.2022.
//

#include "WItemWindow.h"
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

game::ui::windows::WItemWindow::WItemWindow(nk_context** nkContext, GLFWwindow* window, world::ACar* player, game::gamemode::PointGamemode* pointGamemode)
        : nkContext(nkContext), glfwWindow(window), player(player), pointGamemode(pointGamemode)
{
    item = player->getCurrentItem();
}

void game::ui::windows::WItemWindow::init()
{
    item = player->getCurrentItem();
    numberOfViewports = pointGamemode->getNumberOfPlayers();
    id = player->getId();
    glfwGetWindowSize(glfwWindow, &glfwWindowWidth, &glfwWindowHeight);
    windowWidth = numberOfViewports < 3 ? 300.f : 200.f;
    windowHeight = numberOfViewports < 3 ? 100.f : 75.f;
    windowPosX = id == 0 || id == 2 ? (glfwWindowWidth / 2) - windowWidth - 10.f : glfwWindowWidth - windowWidth - 10.f;
    windowPosY = id == 0 || id == 1 ? 0 + 10.f : (glfwWindowHeight / 2) + 10.f;
}

void game::ui::windows::WItemWindow::draw()
{
    init();

    std::string windowID;
    for (int i = 0; i < player->getId(); ++i)
    {
        windowID += " ";
    }


    if (nk_begin(*nkContext, ("    Item " + windowID).c_str(),
                 nk_rect(windowPosX, windowPosY, windowWidth, windowHeight),
                 NK_WINDOW_BORDER | NK_WINDOW_TITLE))
    {
        nk_layout_row_begin(*nkContext, NK_DYNAMIC, 12, 3);
        {
            nk_layout_row_push(*nkContext, 1.f);
            nk_label(*nkContext, ("  " + getItemString(item)).c_str(), NK_TEXT_ALIGN_LEFT);
        }
        nk_layout_row_end(*nkContext);
    }
    nk_end(*nkContext);
}

std::string game::ui::windows::WItemWindow::getItemString(EItem itemToString)
{
    std::string result;
    switch (itemToString)
    {
        case EItem::MANIPULATION:
            result = "Manipulation";
            break;
        case KNOCKBACK_UP:
            result = "Knockback Up";
            break;
        case KNOCKBACK_DOWN:
            result = "Knockback Down";
            break;
        case GHOST:
            result = "Ghost";
            break;
        case NO_ITEM:
            result = "";
            break;
    }

    // Little hack to have basically the same name for every window
    switch (id)
    {
        case 0:
            return result + "";
        case 1:
            return result + " ";
        case 2:
            return result + "  ";
        case 3:
            return result + "   ";
        default:
            return result;
    }
}
