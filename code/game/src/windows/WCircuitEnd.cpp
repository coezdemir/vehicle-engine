//
// Created by Cem on 20.07.2022.
//

#include "WCircuitEnd.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT

#include <nuklear.h>
#include <nuklear_glfw_gl4.h>

#include <utility>

game::ui::windows::WCircuitEnd::WCircuitEnd(nk_context** nkContext, GLFWwindow* window, std::vector<int> pointMap)
        : nkContext(nkContext), glfwWindow(window), points(std::move(pointMap)), winner({0, 0})
{
    for (int i = 0; i < points.size(); ++i)
    {
        if (winner.second < points[i])
            winner = {i, points[i]};
    }
}

void game::ui::windows::WCircuitEnd::init()
{

}

void game::ui::windows::WCircuitEnd::draw()
{
    const float WINDOW_WIDTH = 380.f;
    const float WINDOW_HEIGHT = 170.f;
    const float BUTTON_HEIGHT = 40;
    int glfwWindowWidth, glfwWindowHeight;
    glfwGetWindowSize(glfwWindow, &glfwWindowWidth, &glfwWindowHeight);

    if (nk_begin(*nkContext, "    Winner",
                 nk_rect((glfwWindowWidth / 2.f) - (WINDOW_WIDTH / 2.f),
                         (glfwWindowHeight / 2.f) - (WINDOW_HEIGHT / 2.f), WINDOW_WIDTH, WINDOW_HEIGHT),
                 NK_WINDOW_BORDER | NK_WINDOW_TITLE))
    {
        nk_layout_row_begin(*nkContext, NK_DYNAMIC, 60.f, 1);
        {
            nk_layout_row_push(*nkContext, 1.f);
            nk_label(*nkContext, ("Player " + std::to_string(winner.first + 1)).c_str(), NK_TEXT_ALIGN_CENTERED);
        }
        nk_layout_row_end(*nkContext);

        nk_layout_row_begin(*nkContext, NK_DYNAMIC, BUTTON_HEIGHT, 3);
        {
            nk_layout_row_push(*nkContext, 0.4f);
            nk_label(*nkContext, "", NK_TEXT_ALIGN_LEFT);

            nk_layout_row_push(*nkContext, 0.3f);
            if (nk_button_label(*nkContext, "Restart"))
                event_continue.fire();

            nk_layout_row_push(*nkContext, 0.3f);
            nk_label(*nkContext, "", NK_TEXT_ALIGN_LEFT);
        }
        nk_layout_row_end(*nkContext);
    }
    nk_end(*nkContext);
}


