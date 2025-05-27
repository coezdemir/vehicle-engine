//
// Created by CemOe on 15.08.2021.
//
#include <engine/renderer/Renderer.h>
#include "engine/renderer/manager/TextureManager.h"

engine::renderer::RenderSystem::RenderSystem() : System(), time(0.f)
{

};

void engine::renderer::RenderSystem::registerSystem(engine::Engine* engine)
{
    handle_startup = engine->event_startup.subscribe([this](engine::Engine* engine) {
        startup(engine);
    });

    handle_newFrame = engine->event_newFrame.subscribe([this]() {
        newFrame();
    });

    handle_tick = engine->event_tick.subscribe([this](float deltaTime) {
        tick(deltaTime);
    });

    handle_shutdown = engine->event_shutdown.subscribe([this](engine::Engine* engine) {
        shutdown(engine);
    });
}

void engine::renderer::RenderSystem::startup(engine::Engine* engine)
{
    manager::TextureManager::init();

    // Rendering system started
    meshes = engine->createContainer<engine::renderer::components::CMesh>();

    handle_createMesh = meshes->create_component.subscribe([this](auto meshHandle) {
        meshHandle->shader = &globalShader;
    });

    handle_deleteMesh = meshes->delete_component.subscribe([this](auto meshHandle) {

    });

    models = engine->createContainer<engine::renderer::components::CModel>();

    handle_createModel = models->create_component.subscribe([this](auto modelHandle) {
        modelHandle->shader = &globalShader;
    });

    handle_deleteModel = models->delete_component.subscribe([this](auto modelHandle) {
    });

    cameras = engine->createContainer<engine::renderer::components::CCamera>();

    handle_createCamera = cameras->create_component.subscribe(
            [this, engine](renderer::components::cameraComponent_handle_t cameraHandle) {
                cameraHandle->shader = &globalShader;
                cameraHandle->windowHeight = engine->getContext().getHeight();
                cameraHandle->windowWidth = engine->getContext().getWidth();
            });

    handle_createCamera = cameras->delete_component.subscribe(
            [this](renderer::components::cameraComponent_handle_t cameraHandle) {
            });

    lights = engine->createContainer<engine::renderer::components::CLight>();

    handle_createLight =
            lights->create_component.subscribe([this](renderer::components::light_handle_t lightHandle) {
                lightHandle->shader = &globalShader;
            });

    handle_deleteLight =
            lights->delete_component.subscribe([this](renderer::components::light_handle_t lightHandle) {
            });

    particleEmitters = engine->createContainer<engine::renderer::components::CParticleEmitter>();

    handle_createParticle =
            particleEmitters->create_component.subscribe([this](renderer::components::particle_handle_t particleHandle) {
                particleHandle->shader = &particleShader;
                particleHandle->meshShader = &particleMeshShader;
            });

    handle_deleteParticle =
            particleEmitters->delete_component.subscribe([this](renderer::components::particle_handle_t particleHandle) {
            });
}

void engine::renderer::RenderSystem::newFrame()
{
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void engine::renderer::RenderSystem::tick(const float deltaTime)
{
    time += deltaTime;
    globalShader.use();
    globalShader.setFloat("time", time);
}

void engine::renderer::RenderSystem::shutdown(engine::Engine* engine)
{
    // Unregister system
    engine->event_startup.unsubscribe(handle_startup);
    engine->event_newFrame.unsubscribe(handle_newFrame);;
    engine->event_tick.unsubscribe(handle_tick);
    engine->event_shutdown.unsubscribe(handle_shutdown);

    manager::TextureManager::shutdown();
}

void engine::renderer::RenderSystem::changeShader(const Shader* newShader)
{
    std::for_each(models->begin(), models->end(),
                  [newShader](engine::renderer::components::CModel& meshComponent) {
                      meshComponent.changeShader(newShader);
                  }
    );
}
