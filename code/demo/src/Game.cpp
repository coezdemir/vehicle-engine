//
// Created by CemOe on 18.10.2021.
//

#include "Game.h"

demo::Game::Game() : engine(), world(&engine)
{
    sceneSystem = engine.addSystem<engine::scene::SceneSystem, engine::Engine*>(&engine);
    rendererSystem = engine.addSystem<engine::renderer::RenderSystem>();
    userInterfaceSystem = engine.addSystem<engine::ui::UserInterface>();
    audioSystem = engine.addSystem<engine::audio::Audio>();

    auto sceneDesc = physx::PxSceneDesc(physx::PxTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.kineKineFilteringMode = physx::PxPairFilteringMode::eKEEP;
    sceneDesc.staticKineFilteringMode = physx::PxPairFilteringMode::eKEEP;
    physicsSystem = engine.addSystem<engine::physics::PhysicsSystem, engine::Engine*>(&engine, sceneDesc);

    engine.event_startup.subscribe([this](engine::Engine* engine) {
        startup();
    });

    engine.event_afterStartup.subscribe([this](engine::Engine* engine) {
        afterSystemStartup();
    });

    engine.event_tick.subscribe([this](float deltaTime) {
        tick(deltaTime);
    });

    world.event_keyInput.subscribe([this](const GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_1 && action == GLFW_PRESS)
            switchVehicle(1);
        else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
            switchVehicle(2);
        else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
            switchVehicle(3);
    });
}

void demo::Game::startup()
{

}

void demo::Game::run()
{
    engine.run();
}

void demo::Game::afterSystemStartup()
{
    glm::mat4 transform = glm::mat4(1.f);

    // Light
    transform = glm::mat4(1.f);
    light = world.createActor<engine::renderer::world::ALight>(transform, &world);

    // Camera
    transform = glm::translate(transform, glm::vec3(0.f, 20.f, 50.f));
    camera = world.createActor<engine::renderer::world::ACamera>(transform, &world,
                                                                 []() { return glm::vec3(0.f, 0.f, 0.f); });

    transform = glm::mat4(1.f);
    transform = glm::translate(transform, glm::vec3(0.f, -10.f, 0.f));
    platform = world.createActor<demo::world::APlatform>(transform, &world);


    glm::vec3 cubeOffsets[] =
            {
                    glm::vec3(0.f, 5.f, 0.f),
                    glm::vec3(-2.f, 5.f, 0.f),
                    glm::vec3(2.f, 5.f, 0.f),
                    glm::vec3(-1.f, 8.f, 0.f),
                    glm::vec3(1.f, 8.f, 0.f),
                    glm::vec3(0.f, 10.f, 0.f)
            };

    for (auto& cubeOffset: cubeOffsets)
    {
        transform = glm::mat4(1.f);
        transform = glm::translate(transform, cubeOffset);
        cubes.push_back(world.createActor<demo::world::ACube>(transform, &world));
    }

    transform = glm::mat4(1.f);
    transform = glm::translate(transform, glm::vec3(-20.f, 1.f, 10.f));
    raycastVehicle = world.createActor<demo::world::ADemoRaycastVehicle>(transform, &world);

    transform = glm::mat4(1.f);
    transform = glm::translate(transform, glm::vec3(0.f, 1.f, 10.f));
    fourWheeledVehicle = world.createActor<demo::world::ADemo4WVehicle>(transform, &world);

    transform = glm::mat4(1.f);
    transform = glm::translate(transform, glm::vec3(20.f, 1.f, 10.f));
    tankVehicle = world.createActor<demo::world::ADemoTank>(transform, &world);
}

void demo::Game::tick(float deltaTime)
{

}

void demo::Game::switchVehicle(unsigned int desiredVehicle)
{
    switch(currentVehicle)
    {
        case 1:
            raycastVehicle->deactivateControls();
            break;
        case 2:
            fourWheeledVehicle->deactivateControls();
            break;
        case 3:
            tankVehicle->deactivateControls();
            break;
    }

    switch(desiredVehicle)
    {
        case 1:
            raycastVehicle->activateControls();
            currentVehicle = 1;
            break;
        case 2:
            fourWheeledVehicle->activateControls();
            currentVehicle = 2;
            break;
        case 3:
            tankVehicle->activateControls();
            currentVehicle = 3;
            break;
    }
}
