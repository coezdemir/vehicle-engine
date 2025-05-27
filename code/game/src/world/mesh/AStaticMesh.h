//
// Created by coezdemir on 29.01.2021.
//
#pragma once
#include <engine/renderer/Mesh.h>
#include <engine/renderer/components/CModel.h>
#include <engine/physics/components/collision/CComplexCollision.h>
#include "../AMyActor.h"

namespace game::world
{
    class AStaticMesh : public AMyActor
    {
    public:
        AStaticMesh(glm::mat4 transform, engine::world::World* world, std::string_view filename);

    protected:
        engine::renderer::components::model_handle_t meshComp;

        engine::physics::components::complexCollision_handle_t complexCollisionComp;

        std::vector<std::vector<glm::vec3>> vertices;

        std::vector<std::vector<unsigned int>> indices;
    };
}
