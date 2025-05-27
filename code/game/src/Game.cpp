//
// Created by CemOe on 18.10.2021.
//

#include "Game.h"
#include "world/level/AGoal.h"
#include "levels/Arena01.h"
#include "levels/PhysicsPlayground.h"
#include "levels/Racetrack01.h"
#include "levels/Racetrack02.h"
#include "levels/Racetrack03.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear.h"
#include "nuklear_glfw_gl4.h"

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


game::Game::Game() : gamemode(600, &engine), myCollisionCallback()
{
    sceneSystem = engine.addSystem<engine::scene::SceneSystem, engine::Engine*>(&engine);
    rendererSystem = engine.addSystem<engine::renderer::RenderSystem>();
    userInterfaceSystem = engine.addSystem<engine::ui::UserInterface>();
    audioSystem = engine.addSystem<engine::audio::Audio>();

    auto sceneDesc = PxSceneDesc(PxTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.kineKineFilteringMode = PxPairFilteringMode::eKEEP;
    sceneDesc.staticKineFilteringMode = PxPairFilteringMode::eKEEP;
    sceneDesc.filterShader = game::physics::collision::FilterGroup::GameFilterShader;
    sceneDesc.simulationEventCallback = &myCollisionCallback;
    physicsSystem = engine.addSystem<engine::physics::PhysicsSystem, engine::Engine*>(&engine, sceneDesc);

    userInterfaceSystem->event_skinNuklear.subscribe([](nk_context* nkContext, engine::ui::NuklearGUI* nuklearGUI) {
        nuklearGUI->header = nk_subimage_id(nuklearGUI->skin, 512, 512, nk_rect(28, 27, 337, 33));
        nuklearGUI->window = nk_subimage_id(nuklearGUI->skin, 512, 512, nk_rect(28, 60, 337, 292));
        nuklearGUI->button = nk_subimage_id(nuklearGUI->skin, 512, 512, nk_rect(21, 373, 199, 65));
        nuklearGUI->button_hover = nk_subimage_id(nuklearGUI->skin, 512, 512, nk_rect(221, 373, 199, 65));
        nuklearGUI->button_active = nk_subimage_id(nuklearGUI->skin, 512, 512, nk_rect(21, 373, 199, 65));

        /* window */
        nkContext->style.window.background = nk_rgb(204, 204, 204);
        nkContext->style.window.fixed_background = nk_style_item_image(nuklearGUI->window);
        nkContext->style.window.border_color = nk_rgb(67, 67, 67);
        nkContext->style.window.combo_border_color = nk_rgb(67, 67, 67);
        nkContext->style.window.contextual_border_color = nk_rgb(67, 67, 67);
        nkContext->style.window.menu_border_color = nk_rgb(67, 67, 67);
        nkContext->style.window.group_border_color = nk_rgb(67, 67, 67);
        nkContext->style.window.tooltip_border_color = nk_rgb(67, 67, 67);
        nkContext->style.window.scrollbar_size = nk_vec2(16, 16);
        nkContext->style.window.border_color = nk_rgba(0, 0, 0, 0);
        nkContext->style.window.padding = nk_vec2(8, 4);
        nkContext->style.window.border = 3;

        /* window header */
        nkContext->style.window.header.normal = nk_style_item_image(nuklearGUI->header);
        nkContext->style.window.header.hover = nk_style_item_image(nuklearGUI->header);
        nkContext->style.window.header.active = nk_style_item_image(nuklearGUI->header);
        nkContext->style.window.header.label_normal = nk_rgb(95, 95, 95);
        nkContext->style.window.header.label_hover = nk_rgb(95, 95, 95);
        nkContext->style.window.header.label_active = nk_rgb(95, 95, 95);

        /* default button */
        nkContext->style.button.normal = nk_style_item_image(nuklearGUI->button);
        nkContext->style.button.hover = nk_style_item_image(nuklearGUI->button_hover);
        nkContext->style.button.active = nk_style_item_image(nuklearGUI->button_active);
        nkContext->style.button.border_color = nk_rgba(0, 0, 0, 0);
        nkContext->style.button.text_background = nk_rgba(0, 0, 0, 0);
        nkContext->style.button.text_normal = nk_rgb(95, 95, 95);
        nkContext->style.button.text_hover = nk_rgb(95, 95, 95);
        nkContext->style.button.text_active = nk_rgb(95, 95, 95);
    });

    handle_startup = engine.event_startup.subscribe([this](engine::Engine* engine) {
        startup();
    });

    handle_afterStartup = engine.event_afterStartup.subscribe([this](engine::Engine* engine) {
        afterSystemStartup();
    });

    handle_tick = engine.event_tick.subscribe([this](float deltaTime) {
        tick(deltaTime);
    });

    gamemode.event_finished.subscribe([this]() { loadNextLevel(); });

    gamemode.event_resultsSpawned.subscribe([this]() { pause(); });

    world.event_keyInput.subscribe([this](const GLFWwindow* window, int key, int scancode, int action, int mods) {
        if(key == GLFW_KEY_F1 && action == GLFW_RELEASE)
        {
            bRenderCinematic = !bRenderCinematic;
            sceneSystem->renderCinematic(bRenderCinematic);
        }

        if(key == GLFW_KEY_F2 && action == GLFW_RELEASE)
        {
            gamemode.fireFinishEvent();
        }

        if(key == GLFW_KEY_F3 && action == GLFW_RELEASE)
        {
            if(!debugActor)
            {
                glm::mat4 transform = glm::mat4(1.f);
                transform = glm::translate(transform, glm::vec3(0.f, 5.f, 0.f));
                debugActor = world.createActor<game::world::ABooster>(transform, &world);
            }
        }

        if(key == GLFW_KEY_F4 && action == GLFW_RELEASE)
        {
            if(debugActor)
            {
                debugActor->destroy();
                debugActor.reset();
            }
        }
    });
}

void game::Game::startup()
{

}

void game::Game::run()
{
    engine.run();
}

void game::Game::afterSystemStartup()
{
    printf("-- Startup the **GAME** (after startup event has been fired).\n");
    printf("\n *** HERE THE ACTORS ARE CREATED *** \n\n");

    sceneSystem->setSkybox(&skybox);

    glm::mat4 transform = glm::mat4(1.f);

    currLevel = world.createActor<game::level::Racetrack01>(&world, 0);

    // Cars
    transform = glm::mat4(1.f);
    transform = glm::translate(transform, glm::vec3(20.f, 4.f, 0.f));
    player1 = world.createActor<game::world::ACar>(transform, &world, physicsSystem);
    gamemode.registerPlayer(player1.get());

    transform = glm::translate(transform, glm::vec3(-10.f, 0.f, 0.f));
    player2 = world.createActor<game::world::ACar>(transform, &world, physicsSystem);
    gamemode.registerPlayer(player2.get());

    transform = glm::translate(transform, glm::vec3(-20.f, 0.f, 0.f));
    player3 = world.createActor<game::world::ACar>(transform, &world, physicsSystem);
    gamemode.registerPlayer(player3.get());

    transform = glm::translate(transform, glm::vec3(-10.f, 0.f, 0.f));
    player4 = world.createActor<game::world::ACar>(transform, &world, physicsSystem);
    gamemode.registerPlayer(player4.get());

    myCollisionResponse = game::physics::collision::MyCollisionResponse(&myCollisionCallback, {player1.get(), player2.get(), player3.get(), player4.get()});

    currLevel->loadLevel(&gamemode);

    audioSystem->setBackgroundMusic("music/main-theme.wav", 0.8f);
}

void game::Game::tick(float deltaTime)
{
    gamemode.tick(deltaTime);
}

void game::Game::beforeSystemShutdown()
{

}

void game::Game::loadNextLevel()
{
    currLevel->unloadLevel();
    currLevel->destroy();

    switch (currLevel->getId())
    {
        case 0:
            currLevel =
                    world.createActor<game::level::Racetrack02>(&world, 1);
            break;
        case 1:
            currLevel =
                    world.createActor<game::level::Racetrack03>(&world, 2);
            break;
        default:
            currLevel =
                    world.createActor<game::level::Racetrack01>(&world, 0);
    }

    currLevel->loadLevel(&gamemode);

    for (const std::function<void()>& callback : playerResetCallback)
        callback();

    playerResetCallback.clear();
}

void game::Game::pause()
{
    // Give the callbacks to the next level so it can load them upon initializing physics
    playerResetCallback.push_back(player1->reset());
    playerResetCallback.push_back(player2->reset());
    playerResetCallback.push_back(player3->reset());
    playerResetCallback.push_back(player4->reset());
}
