//
// Created by CemOe on 02.11.2021.
//
#pragma once
#include <engine/scene/components/CRotation.h>
#include "AStaticMesh.h"

namespace game::world
{
    class ARotatingMesh : public AStaticMesh
    {
    public:
        ARotatingMesh(glm::mat4 transform, engine::world::World* world, const char* filename, glm::vec3 axis, float stepSize);

        void tick(float deltaTime) override;

    private:
        engine::scene::components::rotation_handle_t rotationComponent;
    };
}


