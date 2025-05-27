//
// Created by CemOe on 02.11.2021.
//
#pragma once
#include <engine/scene/components/CTranslation.h>
#include "AStaticMesh.h"

namespace game::world
{
    class AMovingMesh : public AStaticMesh
    {
    public:
        AMovingMesh(glm::mat4 transform, engine::world::World* world, std::string_view filename,
                    engine::scene::components::CTranslation::ConstructionInfo translationInfo);

        void tick(float deltaTime) override;

    private:
        engine::scene::components::translation_handle_t translationComponent;
    };
}

