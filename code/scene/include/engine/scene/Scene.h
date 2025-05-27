//
// Created by CemOe on 15.08.2021.
//
#pragma once
#include <engine/system/System.h>
#include <engine/Engine.h>
#include <engine/callbacks/Event.h>
#include <engine/scene/SceneGraph.h>
#include <engine/scene/TreeTraverser.h>
#include <engine/scene/components/CTranslation.h>
#include <engine/scene/components/CRotation.h>
#include <engine/scene/components/CStepRotation.h>
#include <engine/scene/components/CScale.h>

namespace engine::scene
{
    /// \brief Subsystem that holds the scene graph and transform related components
    class SceneSystem final : public engine::system::System
    {
    public:
        explicit SceneSystem(engine::Engine* engine);

        SceneSystem(SceneSystem&&) = delete;

        SceneSystem(const SceneSystem&) = delete;

        /// Remove a CTransform from the scene graph so it does not get traversed anymore.
        /// \param transformID ID of the CTransform to remove
        void removeTransformCompFromSceneGraph(engine::components::id_t transformID);

        /// Set the skybox that should be rendered at the end.
        /// \param skyBox
        void setSkybox(engine::scene::ITraversableObject* skyBox);

        /// Instead of drawing from the current cameras we switch to a cinematic camera.
        /// \param bRenderCinematic
        void renderCinematic(bool bRenderCinematic);

    private:
        /// \brief Registers the system to the engine.
        /// \param engine Pointer to the engine.
        void registerSystem(Engine* engine) override;

        void startup(Engine* engine);

        void tick(float frameTime);

        void shutdown(Engine* engine);

        Engine::event_engineptr_t::handle_t handle_startup;

        Engine::event_engineptr_t::handle_t handle_beginPlay;

        Engine::tick_event_t::handle_t handle_tick;

        Engine::event_engineptr_t::handle_t handle_shutdown;

        scene::SceneGraph sceneTree;

        scene::TreeTraverser traverser;

        scene::components::transform_container_t* transforms;

        scene::components::transform_event_handle handle_createTransform, handle_deleteTransform;

        Engine::event_void_t::handle_t handle_draw;

        scene::components::rotation_container_t* rotationContainer;

        scene::components::rotation_event_handle_t handle_createRotation, handle_deleteRotation;

        scene::components::translation_container_t* translationContainer;

        scene::components::scale_container_t* scaleContainer;

        scene::components::translation_event_handle_t handle_createTranslation, handle_deleteTranslation;

        scene::components::stepRotation_container_t* stepRotationContainer;

        scene::components::stepRotation_event_handle_t handle_createStepRotation, handle_deleteStepRotation;

        scene::components::translation_event_handle_t handle_createScale, handle_deleteScale;
    };
}