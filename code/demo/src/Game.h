//
// Created by CemOe on 18.10.2021.
//
#pragma once
#include <engine/Engine.h>
#include <engine/renderer/Renderer.h>
#include <engine/scene/Scene.h>
#include <engine/physics/Physics.h>
#include <engine/audio/Audio.h>
#include <engine/renderer/world/ALight.h>
#include <engine/renderer/world/ACamera.h>
#include "world/environment/APlatform.h"
#include "world/environment/ACube.h"
#include "world/vehicles/ADemoRaycastVehicle.h"
#include "world/vehicles/ADemo4WVehicle.h"
#include "world/vehicles/ADemoTank.h"

namespace demo
{
    class Game
    {
    public:
        Game();

        void run();

    private:
        void startup();

        void afterSystemStartup();

        void tick(float deltaTime);

        void switchVehicle(unsigned int desiredVehicle);

    private:
        /* Systems */

        engine::Engine engine;

        engine::renderer::RenderSystem* rendererSystem;

        engine::scene::SceneSystem* sceneSystem;

        engine::physics::PhysicsSystem* physicsSystem;

        engine::ui::UserInterface* userInterfaceSystem;

        engine::audio::Audio* audioSystem;

        engine::world::World world;

        /* Actors */

        std::shared_ptr<engine::renderer::world::ALight> light;

        std::shared_ptr<engine::renderer::world::ACamera> camera;

        std::shared_ptr<demo::world::APlatform> platform;

        std::vector<std::shared_ptr<demo::world::ACube>> cubes;

        std::shared_ptr<demo::world::ADemoRaycastVehicle> raycastVehicle;

        std::shared_ptr<demo::world::ADemo4WVehicle> fourWheeledVehicle;

        std::shared_ptr<demo::world::ADemoTank> tankVehicle;

        /* Current vehicle */

        unsigned int currentVehicle = 1;
    };
}