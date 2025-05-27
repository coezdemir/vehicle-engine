//
// Created by CemOe on 27.09.2021.
//
#include <engine/ui/UserInterface.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <engine/ui/windows/WWindow.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL4_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include <nuklear.h>
#include <nuklear_glfw_gl4.h>
#include "stb_image.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

namespace engine::ui
{
    struct NuklearGUI
    {
        GLuint skin;
        struct nk_image menu;
        struct nk_image check;
        struct nk_image check_cursor;
        struct nk_image option;
        struct nk_image option_cursor;
        struct nk_image header;
        struct nk_image window;
        struct nk_image scrollbar_inc_button;
        struct nk_image scrollbar_inc_button_hover;
        struct nk_image scrollbar_dec_button;
        struct nk_image scrollbar_dec_button_hover;
        struct nk_image button;
        struct nk_image button_hover;
        struct nk_image button_active;
        struct nk_image tab_minimize;
        struct nk_image tab_maximize;
        struct nk_image slider;
        struct nk_image slider_hover;
        struct nk_image slider_active;
    };
}

engine::ui::NuklearGUI engine::ui::UserInterface::nuklearGUI = NuklearGUI();
nk_font_atlas engine::ui::UserInterface::atlas = nk_font_atlas();

engine::ui::UserInterface::UserInterface() : System(), bShowDebugOptions(false), nuklearSkinTexture(0), bShowDemo(true)
{

}

void engine::ui::UserInterface::registerSystem(Engine* engine)
{
    handle_startup = engine->event_startup.subscribe([this](engine::Engine* e) {
        startup(e);
    });

    handle_newFrame = engine->event_newFrame.subscribe([this](){
        newFrame();
    });

    handle_draw = engine->event_draw.subscribe([this](){
        draw();
    });

    handle_tick = engine->event_tick.subscribe([this](float deltaTime) {
        tick(deltaTime);
    });

    handle_endFrame = engine->getContext().event_endFrame.subscribe([this]() {
        endFrame();
    });

    handle_shutdown = engine->event_shutdown.subscribe([this](engine::Engine* e) {
        shutdown(e);
    });
}

void engine::ui::UserInterface::startup(engine::Engine* engine)
{
    // Setup  ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void) io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(engine->getContext().getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 460 core");

    // Setup Nuklear
    nkContext = nk_glfw3_init(engine->getContext().getWindow(), NK_GLFW3_INSTALL_CALLBACKS, MAX_VERTEX_BUFFER,
                              MAX_ELEMENT_BUFFER);

    {
        nk_font_atlas* atlas_ptr = &atlas;
        nk_glfw3_font_stash_begin(&atlas_ptr);
        struct nk_font* droid = nk_font_atlas_add_from_file(atlas_ptr, "../../assets/fonts/Digital.ttf", 16, nullptr);
        nk_glfw3_font_stash_end();
        nk_style_set_font(nkContext, &droid->handle);
    }

    skinNuklear();
}

void engine::ui::UserInterface::tick(float deltaTime)
{

}

void engine::ui::UserInterface::shutdown(engine::Engine* engine)
{
    engine->event_startup.unsubscribe(handle_startup);
    engine->event_tick.unsubscribe(handle_tick);
    engine->event_shutdown.unsubscribe(handle_shutdown);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if(nuklearSkinTexture != 0)
    {
        nk_glfw3_destroy_texture(nuklearSkinTexture);
        glDeleteTextures(1, (const GLuint*) &nuklearGUI.skin);
    }

    nk_glfw3_shutdown();
}

void engine::ui::UserInterface::newFrame()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //ImGui::ShowDemoWindow(&bShowDemo);

    // Start the nk frame
    nk_glfw3_new_frame();
}

void engine::ui::UserInterface::endFrame()
{
    for (const std::shared_ptr<windows::WWindow>& w : windowsToRemove)
        windows.remove(w);

    windowsToRemove.clear();
}

void engine::ui::UserInterface::draw()
{
    for (int i = 0; i < windows.count(); ++i)
        windows[i]->draw();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    nk_glfw3_render(NK_ANTI_ALIASING_ON);

    // Restore options after nuklear changes
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_SCISSOR_TEST);
}

void engine::ui::UserInterface::registerWindow(const std::shared_ptr<windows::WWindow>& window)
{
    window->init();
}

void engine::ui::UserInterface::skinNuklear()
{
    // Skinning Nuklear

    // Loading texture
    {
        int x, y, n;
        unsigned char* data = stbi_load("../../assets/textures/gui/nuklear-skin.png", &x, &y, &n, 4);
        if (!data)
        {
            stbi_image_free(data);
            return;
        }

        nuklearSkinTexture = nk_glfw3_create_texture(data, x, y);
        stbi_image_free(data);
    }
    nuklearGUI.skin = nuklearSkinTexture;

    event_skinNuklear.fire(nkContext, &nuklearGUI);
}

bool engine::ui::UserInterface::readdWindowToDrawList(const std::shared_ptr<windows::WWindow>& window)
{
    for(auto& w : windows)
    {
        if(w.get() == window.get())
            return false;
    }

    windows.add(window);

    return true;
}

bool engine::ui::UserInterface::removeWindowFromDrawList(const std::shared_ptr<windows::WWindow>& window)
{
    if(windows.contains(window))
    {
        windowsToRemove.push_back(window);
        return true;
    }

    return false;
}

