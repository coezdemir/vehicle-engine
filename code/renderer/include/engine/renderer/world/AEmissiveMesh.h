//
// Created by Cem on 20.07.2022.
//
#pragma once

#include "AMesh.h"

namespace engine::renderer::world
{
    /// \brief Actor that has just a transform and a mesh that is emissive mesh
    class AEmissiveMesh : public engine::renderer::world::AMesh
    {
    public:
        AEmissiveMesh(glm::mat4 transform, engine::world::World* world,
                      engine::renderer::components::CModel::ConstructionInfo& constructionInfo);
    };
}
