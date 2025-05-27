//
// Created by Cem on 04.04.2022.
//
#pragma once
#include "Level.h"
#include "../gamemode/Gamemode.h"

namespace game::level
{
    class PhysicsPlayground : public game::level::ALevel
    {
    public:
        explicit PhysicsPlayground(engine::world::World* world);

        void loadLevel(gamemode::Gamemode* gamemode) override;

    protected:
        engine::physics::components::boxCollision_handle_t boxCollisionComp;
    };
}
