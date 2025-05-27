//
// Created by Cem on 15.05.2022.
//
#pragma once
#include "ABumper.h"
#include "engine/scene/components/CRotation.h"

namespace game::world
{
    class ARotatingBumper : public game::world::ABumper
    {
    public:
        ARotatingBumper(glm::mat4 transform, glm::mat4 offset, engine::world::World* world,  engine::scene::components::CRotation::ConstructionInfo* rotationInfo);

        void tick(float deltaTime) override;

    private:
        engine::scene::components::transform_handle_t transformOffset;

        engine::scene::components::rotation_handle_t rotationComp;
    };
}