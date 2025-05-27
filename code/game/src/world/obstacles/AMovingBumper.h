//
// Created by Cem on 15.05.2022.
//
#pragma once
#include "ABumper.h"
#include "engine/scene/components/CTranslation.h"

namespace game::world
{
    class AMovingBumper : public game::world::ABumper
    {
    public:
        AMovingBumper(glm::mat4 transform, engine::world::World* world,
                      engine::scene::components::CTranslation::ConstructionInfo* translationInfo);

        void tick(float deltaTime) override;

    private:
        engine::scene::components::translation_handle_t translationComp;
    };
}