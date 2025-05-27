//
// Created by CemOe on 02.12.2021.
//
#pragma once
#include "Level.h"

namespace game::level
{
    class Racetrack01 : public game::level::ALevel
    {
    public:
        explicit Racetrack01(engine::world::World* world, int id)
                : ALevel(world, "game/rt1/Racetrack01-with-mat.obj", id)
        {

        }

    public:
        void loadLevel(gamemode::Gamemode* gamemode) override;
    };
}

