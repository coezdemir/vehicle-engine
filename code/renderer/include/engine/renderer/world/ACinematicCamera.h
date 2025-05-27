//
// Created by CemOe on 24.01.2022.
//
#pragma once
#include <engine/scene/components/CRotation.h>
#include <engine/renderer/components/CCamera.h>
#include <engine/world/AActor.h>

namespace engine::renderer::world
{
    class ACinematicCamera : public engine::world::AActor
    {
    public:
        ACinematicCamera(glm::mat4 transform, glm::mat4 cameraOffset, engine::world::World* world);

        void tick(float deltaTime) override;

    protected:
        engine::scene::components::transform_handle_t transformComp;

        engine::scene::components::transform_handle_t transformOffset;

        engine::renderer::components::cameraComponent_handle_t cameraComp;

        engine::scene::components::rotation_handle_t rotationComponent;

    private:
        std::function<glm::vec3()> targetLocFunction;
    };
}


