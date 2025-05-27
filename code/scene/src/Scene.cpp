//
// Created by CemOe on 15.08.2021.
//
#include <engine/scene/Scene.h>

engine::scene::SceneSystem::SceneSystem(engine::Engine* engine)
        : sceneTree(SceneGraph()), traverser(TreeTraverser(engine))
{

}

void engine::scene::SceneSystem::registerSystem(Engine* engine)
{
    handle_startup = engine->event_startup.subscribe([this](engine::Engine* e) {
        this->startup(e);
    });

    handle_beginPlay = engine->event_beginPlay.subscribe([this](engine::Engine* engine) {
        traverser.initialTraversal(&sceneTree);
    });

    handle_draw = engine->event_draw.subscribe([this](){
        traverser.traverseScene(&sceneTree);
    });

    handle_tick = engine->event_tick.subscribe([this](float deltaTime) {
        this->tick(deltaTime);
    });

    handle_shutdown = engine->event_shutdown.subscribe([this](auto* e) {
        this->shutdown(e);
    });
};

void engine::scene::SceneSystem::startup(Engine* engine)
{
    transforms = engine->createContainer<scene::components::CTransform>();

    handle_createTransform = transforms->create_component.subscribe([this](auto transformHandle) {
        sceneTree.insert(transformHandle->getId());

        transformHandle->event_transformCompAttached.subscribe(
                [transformHandle, this](engine::components::id_t attachedId) {
                    sceneTree.remove(attachedId);
                }
        );
    });

    handle_deleteTransform = transforms->delete_component.subscribe([this](auto transformHandle) {
        sceneTree.remove(transformHandle->getId());

        transformHandle->event_transformCompAttached.unsubscribe();
    });

    rotationContainer = engine->createContainer<scene::components::CRotation>();
    translationContainer = engine->createContainer<scene::components::CTranslation>();
    stepRotationContainer = engine->createContainer<scene::components::CStepRotation>();
    scaleContainer = engine->createContainer<scene::components::CScale>();
}

void engine::scene::SceneSystem::removeTransformCompFromSceneGraph(engine::components::id_t transformID)
{
    sceneTree.remove(transformID);
}

void engine::scene::SceneSystem::tick(const float deltaTime)
{

}

void engine::scene::SceneSystem::shutdown(Engine* engine)
{
    engine->event_startup.unsubscribe(handle_startup);
    engine->event_beginPlay.unsubscribe(handle_beginPlay);
    engine->event_tick.unsubscribe(handle_tick);
    engine->event_shutdown.unsubscribe(handle_shutdown);
    engine->event_draw.unsubscribe(handle_draw);
}

void engine::scene::SceneSystem::setSkybox(engine::scene::ITraversableObject* skyBox)
{
    traverser.setSkybox(skyBox);
}

void engine::scene::SceneSystem::renderCinematic(bool bRenderCinematic)
{
    traverser.setRenderCinematic(bRenderCinematic);
}
