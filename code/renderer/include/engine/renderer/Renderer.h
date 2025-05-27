//
// Created by CemOe on 15.08.2021.
//
#pragma once
#include <engine/system/System.h>
#include <engine/Engine.h>
#include <engine/renderer/components/CModel.h>
#include <engine/renderer/components/CCamera.h>
#include <engine/renderer/components/CLight.h>
#include <engine/renderer/components/CParticleEmitter.h>
#include "engine/renderer/components/CMesh.h"

extern engine::renderer::Shader const globalShader;

namespace engine::renderer
{
    /// \brief Subsystem managing PhysX and its physics bodies
    class RenderSystem final : public engine::system::System
    {
    public:
        RenderSystem();

        RenderSystem(RenderSystem&&) = delete;

        RenderSystem(const RenderSystem&) = delete;

        /// \brief Change the global shader used by all meshes.
        /// \param newShader The new shader that should replace the current glboal shader.
        void changeShader(const Shader* newShader);

    private:
        /// \brief Registers the system to the engine.
        /// \param engine Pointer to the engine.
        void registerSystem(engine::Engine* engine) override;

        void startup(engine::Engine* engine);

        void newFrame();

        void tick(float deltaTime);

        void shutdown(engine::Engine* engine);

    public:
        const Shader globalShader = Shader("../../assets/shaders/phong/blinn-phong-vertex-shader.glsl",
                                           "../../assets/shaders/phong/blinn-phong-fragment-shader.glsl");

        const Shader particleShader = Shader("../../assets/shaders/particle/particle-vertex-shader.glsl",
                                             "../../assets/shaders/particle/particle-fragment-shader.glsl");

        const Shader particleMeshShader = Shader("../../assets/shaders/particle/particle-mesh-vertex-shader.glsl",
                                                 "../../assets/shaders/particle/particle-mesh-fragment-shader.glsl");

        const Shader debugShader = Shader("../../assets/shaders/debug/debugVertexShader.glsl",
                                          "../../assets/shaders/debug/debugFragmentShader.glsl");

    private:
        Engine::event_engineptr_t::handle_t handle_startup;

        engine::Engine::event_void_t::handle_t handle_newFrame;

        Engine::tick_event_t::handle_t handle_tick;

        Engine::event_engineptr_t::handle_t handle_shutdown;

        renderer::components::model_container_t* models;

        renderer::components::mesh_container_t* meshes;

        renderer::components::model_event_handle_t handle_createModel, handle_deleteModel;

        renderer::components::mesh_event_handle_t handle_createMesh, handle_deleteMesh;

        renderer::components::cameraComponent_container_t* cameras;

        renderer::components::cameraComponent_event_handle handle_createCamera, handle_deleteCamera;

        renderer::components::light_container_t* lights;

        renderer::components::light_event_handle handle_createLight, handle_deleteLight;

        renderer::components::particle_container_t* particleEmitters;

        renderer::components::particleContainer_event_handle_t handle_createParticle, handle_deleteParticle;

        float time;
    };
}