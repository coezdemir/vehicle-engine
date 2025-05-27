//
// Created by CemOe on 16.01.2022.
//

#include "WResultsWindow.h"
#include <utility>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT
#include <nuklear.h>

game::ui::windows::WResultsWindow::WResultsWindow(nk_context** nkContext, GLFWwindow* window,
                                                  std::vector<int> points)
        : nkContext(nkContext), glfwWindow(window), points(std::move(points))
{

}


void game::ui::windows::WResultsWindow::init()
{

}

void game::ui::windows::WResultsWindow::draw()
{
    const float WINDOW_WIDTH = 380.f;
    const float WINDOW_HEIGHT = 180.f;
    const float BUTTON_HEIGHT = 40;
    int glfwWindowWidth, glfwWindowHeight;
    glfwGetWindowSize(glfwWindow, &glfwWindowWidth, &glfwWindowHeight);

    if (nk_begin(*nkContext, "    Results",
                 nk_rect((glfwWindowWidth / 2.f) - (WINDOW_WIDTH / 2.f),
                         (glfwWindowHeight / 2.f) - (WINDOW_HEIGHT / 2.f), WINDOW_WIDTH, WINDOW_HEIGHT),
                 NK_WINDOW_BORDER | NK_WINDOW_TITLE))
    {
        for (int i = 0; i < points.size(); i++)
        {
            nk_layout_row_begin(*nkContext, NK_DYNAMIC, 10.f, 3);
            {
                nk_layout_row_push(*nkContext, 0.1f);
                nk_label(*nkContext, "", NK_TEXT_ALIGN_LEFT);

                nk_layout_row_push(*nkContext, 0.35f);
                nk_label(*nkContext, ("Player " + std::to_string(i + 1) + ": ").c_str(), NK_TEXT_ALIGN_LEFT);

                nk_layout_row_push(*nkContext, 0.55f);
                nk_label(*nkContext, std::to_string(points[i]).c_str(), NK_TEXT_ALIGN_LEFT);
            }
            nk_layout_row_end(*nkContext);
        }

        nk_layout_row_begin(*nkContext, NK_DYNAMIC, 10.f, 3);
        {

        }
        nk_layout_row_end(*nkContext);

        nk_layout_row_begin(*nkContext, NK_DYNAMIC, BUTTON_HEIGHT, 3);
        {
            nk_layout_row_push(*nkContext, 0.4f);
            nk_label(*nkContext, "", NK_TEXT_ALIGN_LEFT);

            nk_layout_row_push(*nkContext, 0.3f);
            if (nk_button_label(*nkContext, "Continue"))
                event_continue.fire();

            nk_layout_row_push(*nkContext, 0.3f);
            nk_label(*nkContext, "", NK_TEXT_ALIGN_LEFT);
        }
        nk_layout_row_end(*nkContext);
    }
    nk_end(*nkContext);
}
