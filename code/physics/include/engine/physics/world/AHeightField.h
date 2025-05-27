//
// Created by Cem on 26.06.2022.
//
#pragma once
#include <engine/world/AActor.h>
#include <engine/physics/components/collision/CHeightField.h>
#include <engine/scene/components/CTransform.h>
#include <engine/renderer/components/CModel.h>
#include <engine/renderer/Mesh.h>

namespace engine::physics::world
{
    class AHeightField : public engine::world::AActor
    {
    public:
        AHeightField(glm::mat4 transform, engine::world::World* world);

    protected:
        renderer::Model model;

        engine::scene::components::transform_handle_t transformComp;

        engine::physics::components::heightField_handle_t heightFieldComp;

        engine::renderer::components::model_handle_t meshComp;
    };
}