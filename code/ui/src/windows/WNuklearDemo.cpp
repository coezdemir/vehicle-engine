//
// Created by Cem on 06.01.2022.
//
#include <engine/ui/windows/WNuklearDemo.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT
#include <nuklear.h>

static nk_colorf bg;

engine::ui::windows::WNuklearDemo::WNuklearDemo(GLFWwindow* window, nk_context** nkContext)
        : window(window), nkContext(nkContext)
{

}

void engine::ui::windows::WNuklearDemo::init()
{
    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
}

void engine::ui::windows::WNuklearDemo::draw()
{
    if (nk_begin(*nkContext, "    Demo", nk_rect(50, 50, 300, 400),
                 NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE))
    {
        enum
        {
            EASY, HARD
        };
        static int op = EASY;
        static int property = 20;
        nk_layout_row_static(*nkContext, 30, 80, 1);
        if (nk_button_label(*nkContext, "button"))
            fprintf(stdout, "button pressed\n");

        nk_layout_row_dynamic(*nkContext, 30, 2);
        if (nk_option_label(*nkContext, "easy", op == EASY))
            op = EASY;
        if (nk_option_label(*nkContext, "hard", op == HARD))
            op = HARD;

        nk_layout_row_dynamic(*nkContext, 25, 1);
        nk_property_int(*nkContext, "Compression:", 0, &property, 100, 10, 1);

        nk_layout_row_dynamic(*nkContext, 20, 1);
        nk_label(*nkContext, "background:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(*nkContext, 25, 1);
        if (nk_combo_begin_color(*nkContext, nk_rgb_cf(bg), nk_vec2(nk_widget_width(*nkContext), 400)))
        {
            nk_layout_row_dynamic(*nkContext, 120, 1);
            bg = nk_color_picker(*nkContext, bg, NK_RGBA);
            nk_layout_row_dynamic(*nkContext, 25, 1);
            bg.r = nk_propertyf(*nkContext, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
            bg.g = nk_propertyf(*nkContext, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
            bg.b = nk_propertyf(*nkContext, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
            bg.a = nk_propertyf(*nkContext, "#A:", 0, bg.a, 1.0f, 0.01f, 0.005f);
            nk_combo_end(*nkContext);
        }
    }
    nk_end(*nkContext);
}

