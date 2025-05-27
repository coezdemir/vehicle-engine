//
// Created by CemOe on 09.12.2021.
//
#pragma once
#include <engine/scene/components/CStepRotation.h>
#include "AStaticMesh.h"

namespace game::world
{
    class AStepRotatingMesh : public AStaticMesh
    {
    public:
        AStepRotatingMesh(glm::mat4 transform, engine::world::World* world,
                          std::string_view filename, engine::scene::components::CStepRotation::ConstructionInfo stepRotationInfo);

        void tick(float deltaTime) override;

    private:
        engine::scene::components::stepRotation_handle_t stepRotationComponent;
    };
}
