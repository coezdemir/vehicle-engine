//
// Created by CemOe on 11.01.2022.
//
#include "WStartingWindow.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT
#include <nuklear.h>

game::ui::windows::WStartingWindow:: WStartingWindow(struct nk_context** nkContext, GLFWwindow* window)
        : nkContext(nkContext), window(window)
{

}

void game::ui::windows::WStartingWindow::init()
{

}

void game::ui::windows::WStartingWindow::draw()
{
    const float WINDOW_WIDTH = 380.f;
    const float WINDOW_HEIGHT = 300.f;
    const float BUTTON_HEIGHT = 40;

    glfwGetWindowSize(window, &glfwWindowWidth, &glfwWindowHeight);

    if (nk_begin(*nkContext, "    Welcome",
                 nk_rect((glfwWindowWidth / 2.f) - (WINDOW_WIDTH / 2.f),
                         (glfwWindowHeight / 2.f) - (WINDOW_HEIGHT / 2.f), WINDOW_WIDTH, WINDOW_HEIGHT),
                 NK_WINDOW_BORDER | NK_WINDOW_TITLE))
    {

        (*nkContext)->style.text.color = nk_rgb(255, 255, 255);

        nk_layout_row_dynamic(*nkContext, 10, 1);
        nk_label(*nkContext, "Welcome galactic soldier to the suitability test of the", NK_TEXT_ALIGN_LEFT);

        nk_layout_row_dynamic(*nkContext, 10, 1);
        nk_label(*nkContext, "Galactic Guardians!", NK_TEXT_ALIGN_LEFT);

        nk_layout_row_dynamic(*nkContext, 10, 1);
        nk_label(*nkContext, "The Galactic Guardians are the most respected organization", NK_TEXT_ALIGN_LEFT);

        nk_layout_row_dynamic(*nkContext, 10, 1);
        nk_label(*nkContext, "in the whole galaxy...", NK_TEXT_ALIGN_LEFT);

        nk_layout_row_dynamic(*nkContext, 10, 1);
        nk_label(*nkContext, "If you wish to join us you need to prove your worth!", NK_TEXT_ALIGN_LEFT);

        nk_layout_row_dynamic(*nkContext, 10, 1);
        nk_label(*nkContext, "   You will participate in a number of races, where the winner", NK_TEXT_ALIGN_LEFT);

        nk_layout_row_dynamic(*nkContext, 10, 1);
        nk_label(*nkContext, "   gets the chance to join us.", NK_TEXT_ALIGN_LEFT);

        nk_layout_row_dynamic(*nkContext, 10, 1);
        nk_label(*nkContext, "   Reach the most points of any participant and become", NK_TEXT_ALIGN_LEFT);

        nk_layout_row_dynamic(*nkContext, 10, 1);
        nk_label(*nkContext, "   the WINNER", NK_TEXT_ALIGN_LEFT);

        nk_layout_row_dynamic(*nkContext, 10, 1);
        nk_label(*nkContext, "   You get points by completing a lap or by kicking other", NK_TEXT_ALIGN_LEFT);

        nk_layout_row_dynamic(*nkContext, 10, 1);
        nk_label(*nkContext, "participants off the track.", NK_TEXT_ALIGN_LEFT);

        nk_layout_row_dynamic(*nkContext, 10, 1);
        nk_label(*nkContext, "Good luck soldier.", NK_TEXT_ALIGN_LEFT);

        nk_layout_row_dynamic(*nkContext, 10, 1);
        nk_label(*nkContext, "", NK_TEXT_ALIGN_LEFT);

        nk_layout_row_dynamic(*nkContext, 10, 1);
        nk_label(*nkContext, "", NK_TEXT_ALIGN_LEFT);

        nk_layout_row_begin(*nkContext, NK_DYNAMIC, BUTTON_HEIGHT, 3);
        {
            nk_layout_row_push(*nkContext, 0.4f);
            nk_label(*nkContext, "", NK_TEXT_ALIGN_LEFT);

            nk_layout_row_push(*nkContext, 0.3f);
            if (nk_button_label(*nkContext, "Start"))
                event_continue.fire();

            nk_layout_row_push(*nkContext, 0.3f);
            nk_label(*nkContext, "", NK_TEXT_ALIGN_LEFT);
        }
        nk_layout_row_end(*nkContext);
    }
    nk_end(*nkContext);
}



