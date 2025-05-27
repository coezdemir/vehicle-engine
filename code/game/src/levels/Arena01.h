//
// Created by CemOe on 25.01.2022.
//
#pragma once
#include "Level.h"
#include "../world/mesh/AStaticMesh.h"
#include "../world/mesh/AMovingMesh.h"

namespace game::level
{
    class Arena01 : public game::level::ALevel
    {
    public:
        explicit Arena01(engine::world::World* world, int id)
            : ALevel(world, "arena1/Arena1.obj", id)
        {
            glm::mat4 transform = glm::mat4(1.f);
            actors.push_back(world->createActor<world::AStaticMesh>(transform, world, "../../assets/models/arena1/Arena1_Ramps.obj"));

            engine::scene::components::CTranslation::ConstructionInfo movingMeshInfo;
            movingMeshInfo.pointsToMoveBetween = {glm::vec3(0.f, -25.f, 0.f), glm::vec3(0.f, 25.f, 0.f)};
            movingMeshInfo.speed = 3.f;
            movingMeshInfo.bShouldReverse = true;
            movingMeshInfo.cooldownTimeSec = 1.f;
            actors.push_back(world->createActor<world::AMovingMesh>(transform, world, "../../assets/models/arena1/Arena1_Wall.obj", movingMeshInfo));
        }

        void loadLevel(gamemode::Gamemode* gamemode) override;
    };
}
