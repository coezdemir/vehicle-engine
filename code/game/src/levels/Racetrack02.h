//
// Created by CemOe on 04.12.2021.
//
#pragma once
#include "Level.h"

namespace game::level
{
    class Racetrack02 : public game::level::ALevel
    {
    public:
        explicit Racetrack02(engine::world::World* world, int id)
            : ALevel(world, "game/rt2/Racetrack02-with-mat.obj", id)
        {

        }

        void loadLevel(gamemode::Gamemode* gamemode) override;
    };
}