//
// Created by CemOe on 06.12.2021.
//
#pragma once
#include "Level.h"

namespace game::level
{
    class Racetrack03 : public game::level::ALevel
    {
    public:
        explicit Racetrack03(engine::world::World* world, int id)
            : ALevel(world, "game/rt3/Racetrack03-with-mat.obj", id)
        {

        }

        void loadLevel(gamemode::Gamemode* gamemode) override;
    };
}