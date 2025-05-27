//
// Created by CemOe on 18.10.2021.
//
#pragma once
#include <engine/Engine.h>
#include <engine/renderer/Renderer.h>
#include <engine/scene/Scene.h>
#include <engine/physics/Physics.h>
#include <engine/ui/UserInterface.h>
#include <engine/audio/Audio.h>
#include <memory>
#include "engine/physics/windows/WDebugWindow.h"
#include <engine/renderer/SkyBox.h>
#include "world/ACar.h"
#include "engine/renderer/world/ACamera.h"
#include "engine/scene/components/CRotation.h"
#include "engine/scene/components/CTranslation.h"
#include "engine/scene/components/CStepRotation.h"
#include "world/mesh/AStaticMesh.h"
#include "engine/ui/windows/WNuklearDemo.h"
#include "levels/Level.h"
#include "physics/MyCollisionCallback.h"
#include "physics/MyCollisionReponse.h"
#include "world/level/ABooster.h"

namespace game
{
    class Game
    {
    public:
        Game();

        void run();

        engine::renderer::RenderSystem* rendererSystem;

        engine::scene::SceneSystem* sceneSystem;

        engine::physics::PhysicsSystem* physicsSystem;

        engine::ui::UserInterface* userInterfaceSystem;

        engine::audio::Audio* audioSystem;

    private:
        engine::Engine engine = engine::Engine();

    public:
        engine::world::World world = engine::world::World(&engine);

    private:
        void startup();

        void afterSystemStartup();

        void beforeSystemShutdown();

        void loadNextLevel();

        void pause();

        void tick(float deltaTime);

        engine::Engine::event_engineptr_t::handle_t handle_startup, handle_afterStartup, handle_beginPlay;

        engine::Engine::tick_event_t::handle_t handle_tick;

        ///////////////////////////////////////////////////////////////////////////////

        engine::physics::windows::WDebugWindow::event_t_debugRaycastVehicle::handle_t handle_raycastVehicleUpdate;

        ///////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<engine::ui::windows::WWindow> trackEditor;

        std::vector<std::shared_ptr<game::world::AStaticMesh>> track;

        ///////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<engine::ui::windows::WWindow> nuklearDemo;

        game::ui::windows::WStartingWindow* startingWindow;

        ///////////////////////////////////////////////////////////////////////////////

        engine::physics::PhysicsSystem::event_void_t::handle_t handle_afterPhysicsTick;

        engine::renderer::SkyBox skybox = engine::renderer::SkyBox("../../assets/textures/skybox/space-grey/");

        game::gamemode::PointGamemode gamemode;

        std::shared_ptr<game::level::ALevel> currLevel;

        std::shared_ptr<game::world::ACar> player1, player2, player3, player4;

        std::vector<std::function<void()>> playerResetCallback;

        bool bRenderCinematic = false;

        bool bPhysicsInitialized = false;

        game::physics::collision::MyCollisionCallback myCollisionCallback;

        game::physics::collision::MyCollisionResponse myCollisionResponse;

        std::shared_ptr<game::world::ABooster> debugActor;
    };
}
