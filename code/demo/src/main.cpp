#pragma once
#include "Game.h"

extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

int main()
{
    demo::Game game = demo::Game();
    game.run();
};