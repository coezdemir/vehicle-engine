//
// Created by CemOe on 18.04.2021.
//
#pragma once
#include <engine/world/AActor.h>
#include <engine/renderer/components/CCamera.h>
#include <engine/scene/components/CTransform.h>


namespace engine::renderer::world
{
    class ACamera : public engine::world::AActor
    {
    public:
        ACamera(glm::mat4 transform, engine::world::World* world, const std::function<glm::vec3()>& targetLocFunction);

    protected:
        engine::scene::components::transform_handle_t transformComp;

        engine::renderer::components::cameraComponent_handle_t cameraComp;

    private:
        std::function<glm::vec3()> targetLocFunction;
    };
}

