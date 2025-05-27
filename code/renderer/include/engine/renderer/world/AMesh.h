//
// Created by s369340 on 18.07.2022.
//
#pragma once
#include <engine/world/AActor.h>
#include "engine/renderer/manager/ModelManager.h"
#include "engine/scene/components/CTransform.h"
#include <engine/renderer/components/CModel.h>

namespace engine::renderer::world
{
    /// \brief Actor that has just a transform and a mesh
    class AMesh : public engine::world::AActor
    {
    public:
        AMesh(glm::mat4 transform, engine::world::World* world, std::string_view filename);

    protected:
        engine::scene::components::transform_handle_t transformComp;

        engine::renderer::components::model_handle_t meshComp;
    };
}

